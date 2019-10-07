obj-m += lkm_example.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
test:
	# Colocamos um — na frente do comando rmmod para dizer para ignorar
	# um erro no caso do modulo nao ser carregado.
	-sudo rmmod lkm_example.ko
	sudo dmesg -C
	# Insere o modulo
	sudo insmod lkm_example.ko
	# Exibe o log do kernel
	dmesg