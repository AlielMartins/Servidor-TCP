#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> // socket(); 
#include <sys/types.h>
#include <netinet/in.h> // IPPROTO_TCP
#include <unistd.h> // close();
#include <string.h> // memset();
#include <arpa/inet.h> // inet_ntoa();
#include <pthread.h> // thread();

#define porta_servidor 80
#define qnt_max 255

int Start_Servidor(int port);
int black_list(struct sockaddr_in list);
void * receber_cliente(char * params);
char * filtro_pacotes(char * buffer,size_t tamanho);
char * montar_pacote(char * pagina);
char * proxy(char * pacote,size_t tamanho);

void logs(char * texto);

struct infos_cli{
	int socket;
	struct sockaddr_in settings;
};

void main(int args,char * params[]){
	printf("[!] Inicializando o servidor \n");
	Start_Servidor(porta_servidor); // Inicializa o servidor na porta 80
}

int Start_Servidor(int port){
	pthread_t threads=0;
	int servidor_pointer=0,conexao=0,clients=0,clients_lenght=0;
	struct sockaddr_in settings,clients_settings;
	struct infos_cli cliente_infos;
	// Definições de propriedades de servidor
	servidor_pointer = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	memset(&settings,0,sizeof(settings)); // String!
	settings.sin_family = AF_INET;
	settings.sin_addr.s_addr = INADDR_ANY;
	settings.sin_port = htons(port);
	// --------------------------------------

	if(servidor_pointer == 0){
		printf("[F] Falha em inicializar o socket. \n");
		return 0;
	}else{
		printf("[OK] Servidor inicializado com sucesso. \n");
	}
	conexao = bind(servidor_pointer,(struct sockaddr*)&settings,sizeof(struct sockaddr));
	if(conexao != 0){
		printf("[F] Falha no bind. \n");
		return 0;
	}else{
		printf("[Ok] Bind inicializado com sucesso. \n");		
	}
	listen(servidor_pointer,qnt_max);
	clients_lenght = sizeof(struct sockaddr_in);
	printf("[!] Aguardando clientes [!] \n");
	while(1){
		clients = accept(servidor_pointer,(struct sockaddr*)&clients_settings,&clients_lenght);
		if(clients == -1){
			printf("[F] erro em aceitar cliente! \n");
		}else{
			printf("	[Ok] Cliente %s:%d conectado \n",inet_ntoa(clients_settings.sin_addr),porta_servidor);
			cliente_infos.socket = clients;
			memcpy(&cliente_infos.settings,&clients_settings,sizeof(struct sockaddr));
			if(black_list(clients_settings) != 0){
				pthread_create(&threads,NULL,(void*)receber_cliente,(void*)&cliente_infos);
				pthread_join(threads,NULL);
			}else{
				printf("	[Ok] Cliente %s:%d banido \n",inet_ntoa(clients_settings.sin_addr),porta_servidor);
				close(clients);	
			}
			
		}
	}
	close(servidor_pointer);
}
void * receber_cliente(char * params){ 
	char * buffer;
	size_t tamanho = 1000000,nbytes=0;
	buffer = malloc(tamanho);
	struct infos_cli cliente;
	memcpy(&cliente,params,sizeof(struct infos_cli));
	while(1){
		if( (nbytes = recv(cliente.socket,buffer,tamanho,0)) == 0){
			printf("	[Ok] Cliente %s:%d desconectado \n",inet_ntoa(cliente.settings.sin_addr),porta_servidor);
			free(buffer);
			break;
		}else{
			buffer = filtro_pacotes(buffer,nbytes); 
			if(buffer == NULL){
				free(buffer);
				close(cliente.socket);
				return NULL;
			}
			if(send(cliente.socket,buffer,tamanho,0) == 0){
				printf("[Ok] Cliente com resposta \n"); 
				free(buffer);
				break;
			}
			memset(buffer,0,tamanho);
		}
	}
	close(cliente.socket);
	return NULL;
}
void logs(char * texto){
	FILE * wlogs;
	wlogs = fopen("logs.txt","a+");
	if(wlogs == NULL){
		printf("[F] Falha em escrever no arquivo de log \n");
		return;
	}else{
		fprintf(wlogs,"%s",texto);
	}
	fclose(wlogs);
}

int black_list(struct sockaddr_in list){
	// Função retorna 1 caso permitido e 0 caso o IP esteja banido
	// Black_List do servidor

	printf("	Verificando IP: %s \n",inet_ntoa(list.sin_addr));
	return 1;
}
char * filtro_pacotes(char * buffer,size_t tamanho){
	// Verifica que tipo de pedido é, caso seja aceitavel pelo servidor retorna um ponteiro para resposta!
	printf("Conteudo: %s \n",buffer);
	/* char * paginas = NULL;
	paginas = strtok(buffer,"\n");
	paginas = strtok(&paginas[4]," ");
	char * resposta;
	if(1){ // SE FOR HTTP
		resposta = proxy(montar_pacote(paginas),tamanho);
		if(resposta == NULL){
			printf("[F] Falha na resposta do proxy. \n");
			return NULL; // Retornar pagina de ERROR INTERNO NO PROXY
		}else{
			printf("[Ok] Pagina requisitada devoldida para o cliente. \n");
			return resposta;
		}
	}else{
		printf("[F] Falha no tratamento do pacote. \n");
		return NULL;     // Retornar pagina de ERROR BAD REQUEST
	}
	*/
	return NULL;
}
