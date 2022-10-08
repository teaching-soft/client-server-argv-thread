/***************************************************************************
	file				 : main.c 	
    begin                : 2019
    copyright            : (C) 2019 by Giancarlo Martini and friends
    email                : gm@giancarlomartini.it
                                                                          
    This program is free software; you can redistribute it and/or modify  
    it under the terms of the GNU General Public License as published by  
    the Free Software Foundation; either version 2 of the License, or     
    (at your option) any later version.                                   
                                                                          
*/


#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "server/server_thread.h"

void error_exit(char *msg)
{
	perror(msg);
	exit(1);
}

int init(int argc, char *argv[])
{
	int sock_fd, port_num;
	struct sockaddr_in server_addr;
	/* Controlla che sia stata specificata la porta sulla linea di comando */
	if (argc < 2)
		error_exit("ERRORE, non è stata specificata la porta di ascolto");

	/* Crea il socket */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	/* Controlla che l'operazione sia riuscita */
	if (sock_fd < 0)
		error_exit("ERRORE nel creare il socket:");

	/* Predispone la struttura che conterrà le informazioni del server */
	bzero((char *)&server_addr, sizeof(server_addr));

	/* Prende il numero di porta passato sulla riga di comando,
	   atoi converte la stringa argv[1] in valore numerico */
	port_num = atoi(argv[1]);

	/* Riempe la struttura con le informazioni della connessione
	   Tipo di socket family = AF_INET,
	   Indirizzi su cui restare in ascolto s_addr = INADDR_ANY, potrebbero essere
	   più di uno Porta con relativa conversione sin_port = htons(port_num); La
	   funzione htonl() converte il valore da host order a network order */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port_num);

	/* Collega il socket con informazioni relative alla connessione */
	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		error_exit("ERRORE funzione bind");

	return sock_fd;
}

int main(int argc, char *argv[])
{
	int sock_fd, new_sock_fd, client_len;
	
	/* sockaddr_in: Contiene le informazioni di un host sulla rete */
	struct sockaddr_in client_addr;
	pthread_t handler_thread;
	int n;

	sock_fd = init(argc, argv);

	while (1)
	{
		// Si mette in ascolto in attesa di client che si connettano, la coda è lunga
		// 5 unità 
		listen(sock_fd, 5);

		/* Predispone lo spazio per i dati necessari */
		client_len = sizeof(client_addr);

		/* Accetta la connessione prelevando i dati del client */
		new_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);

		/* Controllo sulla riuscita dell'operazione ... */
		if (new_sock_fd < 0)
			error_exit("ERRORE con la funzione accept");

		if(pthread_create(&handler_thread, NULL, client_connection, &new_sock_fd) != 0) 
                    error_exit("Fallita la creazione del thread");
                
		printf("Avviato threadId: %ld\n",handler_thread);
	}
	// Attende la chiusura di tutti i socket
	pthread_exit(0);
	
	/* Chiude la connessione */
	close(sock_fd);
	
	return 0;
}
