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
	char command[50];
	char json_command[100];
} ultron_command;


//Inicializa os comandos do sistema
ultron_command * InitCommands();

//Coloca os caracteres da string em maiúsculo 
void StrUp(char *str);

//Determina qual comando será enviado para o Kodi
char *WhatCommand(char *voice_recognition, ultron_command *all_commands);


int main(){

	bool ultron_called, command_found;
	//char call_ultron[] = "pocketsphinx_continuous -adcdev plughw:1,0 -lm </path/to/1234.lm> -dict </path/to/1234.dic> -inmic yes";
	char call_ultron[] = "python3 ultron.py";
	char buffer[500];
	char *command, *curl_command;

	ultron_command *all_commands;
	FILE *fp;

	all_commands = InitCommands();

	while(true){		
		ultron_called = false;
		command_found = false;

		fp = popen(call_ultron, "r");
		if (fp==NULL){
			perror("Error!");
			exit(1);
		}
 
		//Verifica se o sistema foi chamado
		while (fscanf(fp, "%s", buffer)!=EOF){
			StrUp(buffer);
			
			if(!(strcmp(buffer, "ULTRON"))){
				system("aplay hello.wav");
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
				if(!(strcmp(buffer, "SHUTDOWN"))){
					exit(0);
				}
		
				command = WhatCommand(buffer, all_commands);
	 		
				if(strcmp(command, "NOT FOUND")){
					command_found = true;
					break;
				}
			}
			pclose(fp);
		
			if(command_found){ 		
				sprintf(curl_command, "curl -X POST -H \"Content-Type: application/json\" -d '%s' http://localhost:8080/jsonrpc", command);
				system(curl_command);
			}
			else{
				system("aplay no_command.wav");
			}
		}
	}

	free(all_commands);
	free(command);
	return 0;
}


ultron_command * InitCommands(){
	
	ultron_command *aux = calloc(16, sizeof(ultron_command *));

	strcpy(aux[0].command, "HOME"); 
	strcpy(aux[0].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Home\"}");
	strcpy(aux[1].command, "UP");
	strcpy(aux[1].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Up\"}");
	strcpy(aux[2].command, "DOWN"); 
	strcpy(aux[2].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Down\"}");
	strcpy(aux[3].command, "LEFT");
	strcpy(aux[3].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Left\"}");
	strcpy(aux[4].command, "RIGHT"); 
	strcpy(aux[4].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Right\"}");
	strcpy(aux[5].command, "SELECT");
	strcpy(aux[5].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Select\"}");
	strcpy(aux[6].command, "BACK"); 
	strcpy(aux[6].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Back\"}");
	strcpy(aux[7].command, "PLAY");
	strcpy(aux[7].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.PlayPause\", \"params\": { \"playerid\": 1 }, \"id\": 1}");
	strcpy(aux[8].command, "PAUSE"); 
	strcpy(aux[8].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.PlayPause\", \"params\": { \"playerid\": 1 }, \"id\": 1}");
	strcpy(aux[9].command, "STOP"); 
	strcpy(aux[9].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Stop\", \"params\": { \"playerid\": 1 }, \"id\": 1}");
	strcpy(aux[10].command, "NEXT");
	strcpy(aux[10].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"next\"}}");
	strcpy(aux[11].command, "PREVIOUS");
	strcpy(aux[11].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"previous\"}}");
	strcpy(aux[12].command, "INCREMENT");
	strcpy(aux[12].json_command, "{ \"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": { \"volume\": \"increment\" }, \"id\": 1 }");
	strcpy(aux[13].command, "DECREMENT");
	strcpy(aux[13].json_command, "{ \"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": { \"volume\": \"decrement\" }, \"id\": 1 }");
	strcpy(aux[14].command, "MUTE");
	strcpy(aux[14].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": {\"volume\":0}, \"id\": 1}");
	strcpy(aux[15].command, "MAXIMUM");
	strcpy(aux[15].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": {\"volume\":100}, \"id\": 1}");

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
	for(i=0; i < 16; i++){
		if(!(strcmp(voice_recognition, all_commands[i].command))){
			aux = calloc(strlen(all_commands[i].json_command), sizeof(char));
			strcpy(aux, all_commands[i].json_command);			
			return aux;
		}
	}

	aux = calloc(10, sizeof(char));
	strcpy(aux, "NOT FOUND");			
	return aux;
}
