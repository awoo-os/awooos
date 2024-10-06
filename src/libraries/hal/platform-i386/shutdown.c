#include "ports.h"
#include <stdio.h>
#include <stddef.h>
#include <hal.h>

void wait_for_keyboard_controller()
{
    while((hal_inb(0x64) & 2) != 0) {
        // Do nothing until the keyboard controller can accept commands.
    }
}

// Hard shutdown: Disable interrupts then do a keyboard RESET.
void hal_shutdown_hard()
{
    puts("\r\nDoing a hard shutdown.\r\n");

    hal_interrupts_disable();

    wait_for_keyboard_controller();
    // Tell the keyboard controller we want to write something..
    hal_outb(0x64, 0xD1);
    // Wait until the keyboard controller isn't busy.
    wait_for_keyboard_controller();
    // Send a RESET command to the keyboard controller.
    hal_outb(0x60, 0xFE);
}

// Clean shutdown.
// TODO: Implement an ACPI-based hal_shutdown().
void hal_shutdown(void)
{
    hal_shutdown_hard();
}

// QEMU-specific shutdown which, if ran in qemu, will return a nonzero
// exit code.
//
// If ran outside of QEMU, it falls back to a hard shutdown.
void hal_shutdown_test_fail(void)
{
    hal_outb(0xf4, 0x00);
    hal_shutdown_hard();
}
