#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

typedef struct {
	char command[50];
	char json_command[100];
} ultron_command;


//Coloca os caracteres da string em maiúsculo 
void StrUp(char *str);


int main()
{
	int i, ultron_called, command_found;
	char call_ultron[] = "pocketsphinx_continuous -inmic yes -lm /home/pi/Desktop/ultron/model/lm/langmodel.lm -dict /home/pi/Desktop/ultron/model/lm/dictionary.dic -hmm /home/pi/Desktop/ultron/model/en-us";
	char buffer[500];
	char command[100], curl_command[500];
	ultron_command all_commands[16];
	
	strcpy(all_commands[0].command, "HOME"); 
	strcpy(all_commands[0].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Home\"}");
	strcpy(all_commands[1].command, "UP");
	strcpy(all_commands[1].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Up\"}");
	strcpy(all_commands[2].command, "DOWN"); 
	strcpy(all_commands[2].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Down\"}");
	strcpy(all_commands[3].command, "LEFT");
	strcpy(all_commands[3].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Left\"}");
	strcpy(all_commands[4].command, "RIGHT"); 
	strcpy(all_commands[4].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Right\"}");
	strcpy(all_commands[5].command, "SELECT");
	strcpy(all_commands[5].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Select\"}");
	strcpy(all_commands[6].command, "BACK"); 
	strcpy(all_commands[6].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Input.Back\"}");
	strcpy(all_commands[7].command, "PLAY");
	strcpy(all_commands[7].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.PlayPause\", \"params\": { \"playerid\": 1 }, \"id\": 1}");
	strcpy(all_commands[8].command, "PAUSE"); 
	strcpy(all_commands[8].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.PlayPause\", \"params\": { \"playerid\": 1 }, \"id\": 1}");
	strcpy(all_commands[9].command, "STOP"); 
	strcpy(all_commands[9].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Player.Stop\", \"params\": { \"playerid\": 1 }, \"id\": 1}");
	strcpy(all_commands[10].command, "NEXT");
	strcpy(all_commands[10].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"next\"}}");
	strcpy(all_commands[11].command, "PREVIOUS");
	strcpy(all_commands[11].json_command, "{\"jsonrpc\":\"2.0\",\"method\":\"Player.GoTo\",\"id\":1,\"params\":{\"playerid\":1,\"to\":\"previous\"}}");
	strcpy(all_commands[12].command, "INCREMENT");
	strcpy(all_commands[12].json_command, "{ \"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": { \"volume\": \"increment\" }, \"id\": 1 }");
	strcpy(all_commands[13].command, "DECREMENT");
	strcpy(all_commands[13].json_command, "{ \"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": { \"volume\": \"decrement\" }, \"id\": 1 }");
	strcpy(all_commands[14].command, "MUTE");
	strcpy(all_commands[14].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": {\"volume\":0}, \"id\": 1}");
	strcpy(all_commands[15].command, "MAXIMUM");
	strcpy(all_commands[15].json_command, "{\"jsonrpc\": \"2.0\", \"method\": \"Application.SetVolume\", \"params\": {\"volume\":100}, \"id\": 1}");

	
	FILE *fp;

	fp = popen(call_ultron, "r");
	if(fp==NULL)
	{
		perror("Error!");
		exit(1);
	}


	while(TRUE)
	{		
		ultron_called = FALSE;
		command_found = FALSE;
 
		//Verifica se o sistema foi chamado
		while (TRUE)
		{
			fscanf(fp, "%s", buffer);
			StrUp(buffer);
			if(!(strcmp(buffer, "ULTRON")))
			{

				system("aplay hello.wav");
				ultron_called = TRUE;
				break;
			}			
		}
		

		//Caso o sistema tenha sido chamado, verifica o comando
		if(ultron_called)
		{
			fscanf(fp, "%s", buffer);
			StrUp(buffer);

			if(!(strcmp(buffer, "SHUTDOWN")))
			{
				pclose(fp);
				exit(0);
			}
	
			for(i=0; i < 16; i++)
			{
				if(!(strcmp(buffer, all_commands[i].command)))
				{
					strcpy(command, all_commands[i].json_command);
					command_found = TRUE;			
					break;
				}		
			}
	
			if(command_found)
			{ 		
				sprintf(curl_command, "curl -X POST -H \"Content-Type: application/json\" -d '%s' localhost:8080/jsonrpc", command);
				system(curl_command);
			}
			
			else{
				system("aplay no_command.wav");
			}
		}
	}
	
	pclose(fp);
	return 0;
}



void StrUp(char *str)
{
	while(*str)
	{
		*str = toupper(*str);
		str++;
	}
}
