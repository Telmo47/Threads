🧵 Produtor-Consumidor com Threads e Buffer Circular (C)

Este projeto em C implementa o clássico problema do Produtor-Consumidor, utilizando threads POSIX (pthread), mutexes e variáveis de condição para coordenar a produção e o consumo de dados num buffer circular partilhado.
🛠️ Funcionalidades

    4 produtores e 2 consumidores executam em paralelo.

    Os produtores geram números inteiros sequenciais (10001 a 20000) e inserem-nos no buffer circular (RING_SIZE = 15).

    Os consumidores retiram valores do buffer, aplicam módulo % 10000 e armazenam o resultado num final_buffer de tamanho 10000.

    Uso de mutex para garantir exclusão mútua e variáveis de condição not_full / not_empty para sincronização.

    No final, os dados do buffer final são ordenados com qsort e é feita uma estatística simples por thread.

🔎 Objetivo Didático

Este código é ideal para compreender:

    Sincronização entre threads.

    Utilização prática de buffers circulares.

    Evitar condições de corrida com mecanismos corretos de locking.

    Coordenação entre múltiplos produtores e consumidores em sistemas concorrentes.
