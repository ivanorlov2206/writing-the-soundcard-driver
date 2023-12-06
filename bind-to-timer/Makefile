obj-m += timerbind.o

KSRC := ~/linux_work/linux-sound/

all:
	make -C $(KSRC) M=$(PWD) modules
clean:
	rm -rf *.o *.mod.o *.ko
