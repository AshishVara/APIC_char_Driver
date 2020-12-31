# APIC_char_Driver
Character Driver that supports open, release and ioctl operations on advanced programmble Interrupt controller(IOAPIC)

Implement ioctl operation for the following:

        1. APIC_GETID           --> returns identification number of APIC
        2. APIC_GETIRQS         --> returns number of irq pins on APIC
        3. APIC_GETIRQSTATUS    --> returns irq pin status(enable or disable)
        4. APIC_GETIRQTYPE      --> returns irq pin signal type(level or edge trigger)
