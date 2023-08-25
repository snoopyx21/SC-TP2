#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

void SIGINT_recu(int val)
{
}

void demande_arret_thread(int val)
{
}

void* fct_threads(void * arg)
{
    pthread_barrier_t *barr = (pthread_barrier_t *) arg;
    sigset_t masque;

    if(sigemptyset(&masque)!=0)
    {
        perror("Error sigemptyset");
        exit(10);
    }

    if(sigaddset(&masque, SIGINT)!=0)
    {
        perror("Error sigaddset");
        exit(7);
    }

    if(pthread_sigmask(SIG_SETMASK, &masque, NULL)!=0)
    {
        perror("Error pthread_sigmask");
        exit(8);
    }

    pthread_barrier_wait(barr);
    
    pause();
    fprintf(stdout,"Un tread a reçu un signal et sort de pause\n");
    return NULL;

}

int main(int argc, char* argv[])
{
    struct sigaction sig;
    pthread_t *tid;
    int nb_threads, i;
    pthread_barrier_t barr;
    sigset_t masque;

    if(argc<2)
    {
        fprintf(stderr,"Utilisation : ./Exo1 nb_threads\n");
        exit(2);
    }
    
    nb_threads = atoi(argv[1]);
    
    tid = malloc(nb_threads*sizeof(pthread_t));

    if(sigemptyset(&masque)!=0)
    {
        perror("Error sigemptyset");
        exit(9);
    }

    sig.sa_handler = SIGINT_recu;
    sig.sa_mask = masque;
    sig.sa_flags = 0;
    
    if(sigaction(SIGINT, &sig, NULL)!=0)
    {
        perror("Error sigaction");
        exit(3);
    }
    
    sig.sa_handler = demande_arret_thread;
    
    if(sigaction(SIGUSR1, &sig, NULL)!=0)
    {
        perror("Error sigaction");
        exit(4);
    }
    
    pthread_barrier_init(&barr, NULL, nb_threads+1);

    for(i=0; i<nb_threads; i++)
    {
        if((errno=pthread_create(&tid[i],NULL,fct_threads,&barr))!=0)
        {
            perror("Error create");
            exit(5);
        }
    }
    
    pthread_barrier_wait(&barr); //TODO verifier la valeur de retour des fcts barrieres
    
    for(i=0; i<nb_threads; i++)
    {
        pause();
        fprintf(stdout,"\nLe signal SIGINT e#st reçu, envoie de SIGUSR1 au thread numero %d\n",i);
        pthread_kill(tid[i], SIGUSR1);
        //TODO Deplacer le pthread_join ici pour eviter de faire attendre les threads finis
    }
    
    for(i=0; i<nb_threads; i++)
    {
        if((errno=pthread_join(tid[i],NULL))!=0)
        {
            perror("Error create");
            exit(6);
        }
    }
    fprintf(stdout, "Tout les threads sont terminés\n");
    
    pthread_barrier_destroy(&barr);
    free(tid);
    
    return 0;   
}
