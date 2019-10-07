# Simples módulo de dispositivo do kernel do linux
**Utilizando um dispositivo de caracteres**

Tutorial criado por Robert W. Oliver II e que pode ser encontrado atraves deste [link](https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234).

Para executar basta clonar o repositório, abri-lo e rodar os comandos abaixo:

```make```

Em seguida executar:

```make test```

Após executar o comando acima, você obterá um valor, pegue esse valor e substitua no lugar do MAJOR no comando abaixo:

```
sudo mknod /dev/lkm_example c MAJOR 0
```

Para pegar o conteudo do dispositivo voce pode executar:

```
cat /dev/lkm_example
```

ou

```
dd if=/dev/lkm_example of=test bs=14 count=100
```

Quando finalizar, exclua e descarregue o módulo (Se o comando utilizando o cat foi executado e esteja demorando a executar, basta aperta ctrl + c para forçar a parada na execução):

```
sudo rm /dev/lkm_example
sudo rmmod lkm_example
```