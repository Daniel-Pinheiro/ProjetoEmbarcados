#include <stdio.h>
#include <stdlib.h>

FILE* read_file(char *nome_arquivo)
{
    FILE *arquivo;
    if( !(arquivo = fopen(nome_arquivo,"r")) )
    {
        printf("Erro! Impossivel abrir o arquivo!\n");
        exit(1);
    }
    return arquivo;
}
