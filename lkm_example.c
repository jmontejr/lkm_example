#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jose Monte");
MODULE_DESCRIPTION("Um simples exemplo de um modulo de caracter linux.");
MODULE_VERSION("0.01");

#define DEVICE_NAME "lkm_example"
#define EXAMPLE_MSG "Ola, mundo!\n"
#define MSG_BUFFER_LEN 15

/* Prototipos para funcoes do dispositivo */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;

/* Esta estrutura aponta para todas as funcoes do dispositivo */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* Quando um processo e lido do nosso dispositivo, isto e chamado */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    int bytes_read = 0;

    /* Se nos estivermos no final, volta ao inicio */
    if (*msg_ptr == 0) {
        msg_ptr = msg_buffer;
    }

    /* Coloca os dados no buffer */
    while (len && *msg_ptr) {
        /**
         * O buffer esta nos dados do usuario, nao no kernel,
         * portanto na da apenas para fazer a referencia
         * a um ponteiro. A funcao put_user foi criado para lidar com isso
         */
        put_user(*(msg_ptr++), buffer++);
        len--;
        bytes_read++;
    }
    return bytes_read;
}

/* Chamado quando um processo tenta gravar em nosso dispositivo */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    /* Este e um dispositivo somente para leitura */
    printk(KERN_ALERT "Esta operacao nao e suportada.\n");
    return -EINVAL;
}
/* Chamado quando um processo abre o nosso dispositivo */
static int device_open(struct inode *inode, struct file *file) {
    /* Se o dispositivo esta aberto, retorne ocupado */
    if (device_open_count) {
        return -EBUSY;
    }

    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

/* Chamado quando um processo fecha o nosso dispositivo */
static int device_release(struct inode *inode, struct file *file) {
    /**
     * Decrementa o contador aberto e a contagem de uso.
     * Sem isso o modulo nao descarregaria. 
     */
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init lkm_example_init(void) {
    /* Preenche o buffer com a mensagem */
    strncpy(msg_buffer, EXAMPLE_MSG, MSG_BUFFER_LEN);

    /* Configura a msg_ptr para o buffer */
    msg_ptr = msg_buffer;

    /* Tenta registrar o disposito de caracter */
    major_num = register_chrdev(0, "lkm_example", &file_ops);

    if (major_num < 0) {
        printk(KERN_ALERT "Nao foi possivel registrar o dispositivo: %d\n", major_num);
        return major_num;
    } else {
        printk(KERN_INFO "modulo lkm_example carregado com o major number %d\n", major_num);
        return 0;
    }
}

static void __exit lkm_example_exit(void) {
    /**
     * Deve-se limpar o registro manualmente, o kernel nao fara isso.
     * Cancelar o registro do dispositivo. 
     */
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Adeus, mundo!\n");
}

/* Registrar funcoes do modulo */
module_init(lkm_example_init);
module_exit(lkm_example_exit);