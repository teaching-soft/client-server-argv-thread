/***************************************************************************
	file				 : server_thread.c 	
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
#include <string.h>

void *client_connection(void* socket_id)
{
	char buffer[256];
	// Prima lo converto in un puntatore a int
	int *p_new_socket_id = (int *)socket_id;
	
	// Poi lo deferenzio
	int new_socket_id = *p_new_socket_id;
	
	pthread_t t_id = pthread_self();
	printf("Sono nel thread id:%ld Socket:%d\n",t_id, new_socket_id);
	int exit = 0;
	do
	{
		// Predispone il buffer per i dati, azzerandolo
		bzero(buffer, 256);

		// Legge i dati arrivati
		// int n = read(new_socket_id, buffer, 255);
		int n = recv(new_socket_id, buffer, 255,0);
		if (n < 0)
			perror("ERRORE leggendo dal socket");
		if (n == 0){
			printf("Il client ha chiuso la connessione.");
			break;
		}
		
		// Stampa i dati arrivati
		printf("T_ID %d - Lunghezza messaggio %d - Questo è il messaggio ricevuto: %s\n",t_id, strlen(buffer),buffer);


		char msg[100];
		if(buffer[0] == 'Q') {
			strcpy(msg,"Messaggio DI USCITA ricevuto.\nEsco");
			exit = 1;
		}
		else strcpy(msg, "Messaggio ricevuto");

		// Invia il messaggio di avvenuta ricezione
		n = write(new_socket_id, msg, strlen(msg));

		if (n < 0)
			perror("ERRORE scrivendo nel socket");
	} while (exit == 0);

	usleep(100);
	// Chiude la connessione
	close(new_socket_id);
	printf("T_ID %d - Fine thread\n",t_id);
	pthread_exit(0);
}
