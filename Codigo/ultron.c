#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>

typedef struct {
	char *command;
	char *json_command;
} ultron_command;


//Inicializa os comandos do sistema
ultron_command * InitCommands();

//Coloca os caracteres da string em maiúsculo 
void StrUp(char *str);

//Determina qual comando será enviado para o Kodi
char *WhatCommand(char *voice_recognition, ultron_command *all_commands);

//Realiza o controle externo (TCP) pela rede no Kodi
int sendCommandTCP (char *IP_Servidor, unsigned short servidorPorta, char *mensagem );


int main(){

	bool ultron_called = false;
//	char call_ultron[] = "pocketsphinx_continuous -adcdev plughw:1,0 -lm </path/to/1234.lm> -dict </path/to/1234.dic> -inmic yes";
	char call_ultron[] = "python3 ultron.py";
	char buffer[100];
	char *command;

	ultron_command *all_commands;
	FILE *fp;

	all_commands = InitCommands();

	while(true){
			
		ultron_called = false;

		fp = popen(call_ultron, "r");
		if (fp==NULL){
			perror("Error!");
			exit(1);
		}
 
		//Verifica se o sistema foi chamado
		while (fscanf(fp, "%s", buffer)!=EOF){
			StrUp(buffer);
			if(!(strcmp(buffer, "NUMBER"))){
				system("aplay -D plughw:0,0 hello.wav");
				ultron_called = true;
				break;
			}			
		}
		pclose(fp);
		
		//Caso o sistema tenha sido chamado, verifica o comando
		if(ultron_called){

			fp = popen(call_ultron, "r");
			if (fp==NULL){
				perror("Error!");
				exit(1);
			} 

			while(fscanf(fp, "%s", buffer)!=EOF){

				StrUp(buffer);
				command = WhatCommand(buffer, all_commands);
				
				if(!(strcmp(command, "SHUTDOWN"))){
					exit(0);
				}
				else if(strcmp(command, "NOT FOUND")){
					break;
				}
			}
			pclose(fp);
	 		
			sendCommandTCP ("127.0.0.1", 9090, command);
		}
	}

	free(all_commands);
	free(command);
	return 0;
}



ultron_command * InitCommands(){
	
	ultron_command *aux = malloc(15*sizeof(ultron_command *));

	aux[0].command = "HOME"; 
	aux[0].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Home\"}";
	aux[1].command = "UP";
	aux[1].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Up\"}";
	aux[2].command = "DOWN"; 
	aux[2].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Down\"}";
	aux[3].command = "LEFT" ;
	aux[3].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Left\"}";
	aux[4].command = "RIGHT"; 
	aux[4].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Right\"}";
	aux[5].command = "SELECT";
	aux[5].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Select\"}";
	aux[6].command = "BACK"; 
	aux[6].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Back\"}";
	aux[7].command = "PLAY";
	aux[7].json_command = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.PlayPause\", \"params\": { \"playerid\": 1 }, \"id\": 1}";
	aux[8].command = "PAUSE"; 
	aux[8].json_command = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.PlayPause\", \"params\": { \"playerid\": 1 }, \"id\": 1}";
	aux[9].command = "STOP"; 
	aux[9].json_command = "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Stop\", \"params\": { \"playerid\": 1 }, \"id\": 1}";
	aux[10].command = "NEXT";
	aux[10].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"next\"}}";
	aux[11].command = "PREVIOUS";
	aux[11].json_command = "{\"jsonrpc\":\"2.0\",\"method\":\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"previous\"}}";
	aux[12].command = "INCREMENT";
	aux[12].json_command = "{ \"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": { \"volume\": \"increment\" }, \"id\": 1 }";
	aux[13].command = "DECREMENT";
	aux[13].json_command = "{ \"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": { \"volume\": \"decrement\" }, \"id\": 1 }";

	return aux;
}


void StrUp(char *str){
	while(*str){
		*str = toupper(*str);
		str++;
	}
}


char *WhatCommand(char *voice_recognition, ultron_command *all_commands){

	char *aux;
	int i;
	
	StrUp(voice_recognition);
	for(i=0; i < 15; i++){
		if(strcmp(voice_recognition, all_commands[i].command)){
			aux = malloc(sizeof(char)*strlen(all_commands[i].json_command));
			strcpy(aux, all_commands[i].json_command);			
			return aux;
		}
	}

	aux = malloc(sizeof(char)*10);
	strcpy(aux, "NOT FOUND");			
	return aux;
}


int sendCommandTCP (char *IP_Servidor, unsigned short servidorPorta, char *mensagem ){
	int socket_id;
	struct sockaddr_in servidorAddr;
	int length;

	fprintf(stderr, "Abrindo o socket para o cliente... ");
	socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(socket_id < 0){
		fprintf(stderr, "Erro na criacao do socket!\n");
		exit(0);
	}
	fprintf(stderr, "Feito!\n");
	
	fprintf(stderr, "Conectando o socket ao IP %s pela porta %d... ", IP_Servidor, servidorPorta);
	memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
	servidorAddr.sin_family = AF_INET;
	servidorAddr.sin_addr.s_addr = inet_addr(IP_Servidor);
	servidorAddr.sin_port = htons(servidorPorta);

	if(connect(socket_id, (struct sockaddr *) &servidorAddr, sizeof(servidorAddr)) < 0){
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
