/*
 * Copyright 2024, Jean-Christophe Dubois <jcd@tribudubois.net>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <devices_gen.h>
#include <drivers/common.h>
#include <drivers/uart.h>

#include <elfloader_common.h>

#define THR        0x0
#define RHR        0x0
#define LSR        0x5

#define LSR_THRE   BIT(5)
#define LSR_RDR    BIT(0)

#define UART_REG(mmio, x) ((volatile uint32_t *)(mmio + (x)))

static int ns16550_uart_putchar(struct elfloader_device *dev, unsigned int c)
{
    volatile void *mmio = dev->region_bases[0];

    /* Wait to be able to transmit. */
    while (!(*UART_REG(mmio, LSR) & LSR_THRE));

    /* Transmit. */
    *UART_REG(mmio, THR) = c;

    return 0;
}

static int ns16550_uart_init(struct elfloader_device *dev, UNUSED void *match_data)
{
    uart_set_out(dev);
    return 0;
}

static const struct dtb_match_table ns16550_uart_matches[] = {
    { .compatible = "fsl,ns16550" },
    { .compatible = "ns16550a" },
    { .compatible = NULL /* sentinel */ },
};

static const struct elfloader_uart_ops ns16550_uart_ops = {
    .putc = &ns16550_uart_putchar,
};

static const struct elfloader_driver ns16550_uart = {
    .match_table = ns16550_uart_matches,
    .type = DRIVER_UART,
    .init = &ns16550_uart_init,
    .ops = &ns16550_uart_ops,
};

ELFLOADER_DRIVER(ns16550_uart);
