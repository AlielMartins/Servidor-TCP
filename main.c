#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct clients_infos{									// Struct necessaria para passar informações para thread
    int socket_id;
    struct sockaddr settings;
};

void log(char * txt);									// Função que escreve no arquivo de LOG
int Start_Server(int port);								// Função que inicializa o servidor na porta passada como (int)parametro
void * clients(void * params);							// Função que trata o cliente
int  firewall(void * infos);							// Função que verifica se  o cliente pode se comunicar com servidor (1-Autorizado,0-Acesso Negado)

int main(int args, char * params[]){
    Start_Server(80);                                                                   // Inicialização de servidor na porta 80
}

int Start_Server(int port){
    int Socket = 0,connection=0,clients_socket=0;
    pthread_t threads=0;
    socklen_t clients_size=0;
    struct sockaddr_in Settings;                                                        // Struct com configurações do servidor
    struct sockaddr_in clients_settings;                                                // Struct para receber informações do cliente
    struct clients_infos infos_clients;                                                 // Struct para passar informações do cliente
    memset(&Settings,0,sizeof(Settings));                                               // Limpa a struct com configurações do servidor
    memset(&clients_settings,0,sizeof(clients_settings));                               // Limpa a struct com configurações do cliente
    memset(&infos_clients,0,sizeof(infos_clients));                                     // Limpa a struct com informações do cliente
    Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);                                   // Define regras da conexão
    if(Socket == -1){                                                                   // Verifica se o socket foi definido com sucesso
        printf("[F] Falha em definir as configurações de  socket. \n");
        return 0;
    }
    log((char*)"[SOCKET] Socket definido com sucesso. \n");                             // Adiciona no arquivo de log
    Settings.sin_family = AF_INET;                                                      // Define a familia do servidor
    Settings.sin_addr.s_addr = INADDR_ANY;                                              // Inicializa o servidor no IP principal da maquina
    Settings.sin_port = htons(80);                                                      // Define a porta a ser utilizada
    if( (connection = bind(Socket,(struct sockaddr*)&Settings,sizeof(Settings)))==-1){  // Inicializa o bind e verifica se foi realizado com sucesso
        printf("[F] Falha em inicializar o bind. \n");
        return 0;
    }
    log((char*)"[SOCKET] Bind inicializado com sucesso \n");                            // Adiciona no arquivo de log
    listen(Socket,255);                                                                 // Quantidade de conexões suportadas pelo Socket
    printf("[Ok] Aguardando Conexão \n");
    clients_size = sizeof(struct sockaddr_in);
    while(1){
        clients_socket = accept(Socket,(struct sockaddr*)&clients_settings,&clients_size);      // Estabelece conexão com cliente
        if(clients_socket == -1){
            printf("[F] Falha em estabelecer conexão com cliente \n");
        }
        printf("[Ok] Cliente %s:%d conectado com o servidor. \n",inet_ntoa(clients_settings.sin_addr),port);
        infos_clients.socket_id = clients_socket;                                       // Passa valor do socket do cliente
        memcpy(&infos_clients.settings,&clients_settings,sizeof(clients_settings));     // Passa configurações  (IP,PORTA,FAMILIA)
        if(firewall(&infos_clients)){
	        pthread_create(&threads,NULL,&clients,&infos_clients);
	       //pthread_join(threads,NULL);
        }else{
        	printf("[F] Cliente %s não autorizado\nDesconectado!!\n",inet_ntoa(clients_settings.sin_addr));
        	close(clients_socket);
        }
    }
    close(Socket);                                                                      // fecha o socket
}
void log(char * txt){
    FILE * wLogs;                                                                       // Aponta para endereço do arquivo
    wLogs = fopen("logs.txt","a");                                                      // Abre o arquivo logs.txt caso não exista seja criado
    if(wLogs == NULL){                                                                  // Verifica se criou ou abrio com sucesso
        printf("[F] Falha em abrir o arquivo de log. \n");
        return;
    }
    fprintf(wLogs,"%s",txt);                                                            // escreve os dados no arquivo
}
void * clients(void * params){                                                          // Função que executa funções para o cliente
    struct clients_infos infos_clientes;
    memcpy(&infos_clientes,params,sizeof(infos_clientes));                              // Recupera os dados do cliente

    // CODIGO PARA INTERPRETAR OS PACOTES TCP/TCP RECEBIDOS PELO CLIENTE

    // ------------------------------------------------------------------
    close(infos_clientes.socket_id);                                                    // Finaliza conexão com o cliente
    return NULL;
}

int  firewall(void * infos){															// Verifica se o cliente pode entrar na rede
	return 1;
}