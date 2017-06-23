#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>

// Tem que habilitar a controle externo pela rede no Kodi


int sendCommandTCP (char *IP_Servidor, unsigned short servidorPorta, char *mensagem )
{
	int socket_id;
	struct sockaddr_in servidorAddr;
	int length;

	fprintf(stderr, "Abrindo o socket para o cliente... ");
	socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socket_id < 0)
	{
		fprintf(stderr, "Erro na criacao do socket!\n");
		exit(0);
	}
	fprintf(stderr, "Feito!\n");
	
	fprintf(stderr, "Conectando o socket ao IP %s pela porta %d... ", IP_Servidor, servidorPorta);
	memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
	servidorAddr.sin_family = AF_INET;
	servidorAddr.sin_addr.s_addr = inet_addr(IP_Servidor);
	servidorAddr.sin_port = htons(servidorPorta);
	if(connect(socket_id, (struct sockaddr *) &servidorAddr, 
							sizeof(servidorAddr)) < 0)
	{
		fprintf(stderr, "Erro na conexao!\n");
		exit(0);
	}
	fprintf(stderr, "Feito!\n");

	fprintf(stderr, "Mandando mensagem ao servidor... ");
	length = strlen(mensagem) + 1;
	write(socket_id, &length, sizeof(length));
	write(socket_id, mensagem, length);
	fprintf(stderr, "Feito!\n");

	fprintf(stderr, "Fechando o socket local... ");
	close(socket_id);
	fprintf(stderr, "Feito!\n");
	return 0;
}

int main()
{
    sendCommandTCP ( "127.0.0.1", 9090, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.PlayPause\", \"params\": { \"playerid\": 0 }, \"id\": 1}" );
}
