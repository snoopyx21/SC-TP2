#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <pthread.h>

#define N 4


long int compteur=0;


void * incremente(void * verrou)
{
	int i;
	for(i=0;i<1000000;i++)
	{
		
      	pthread_mutex_lock ((pthread_mutex_t*)verrou);
		compteur++;
		pthread_mutex_unlock ((pthread_mutex_t*)verrou);
	}
	return NULL;
}












int main(void)
{
	pthread_t tid[N];
	int i;
	pthread_mutex_t verrou;

	
	pthread_mutex_init(&verrou,NULL);
	for(i=0;i<N;i++)
	{
		if((pthread_create(&tid[i],NULL,incremente,&verrou))==-1)
		{
			perror("pthread_create");
			exit(1);
		}
	}
	
	for(i=0;i<N;i++)
	{
		if((pthread_join(tid[i],NULL))==-1)
		{
			perror("pthread_join");
			exit(1);
		}
	}
	
	printf("On aimerait 4 millions : %ld\n",compteur);
	return 0;
}
	
		
