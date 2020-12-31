#define MAGIC_NO 'A'

#define APIC_GETID          _IOR(MAGIC_NO, 1, unsigned int *)
#define APIC_GETIRQS        _IOR(MAGIC_NO, 2, unsigned int *)
#define APIC_GETIRQSTATUS   _IOWR(MAGIC_NO, 3, unsigned int *)
#define APIC_GETIRQTYPE     _IOWR(MAGIC_NO, 4, unsigned int *)

