#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stack>
#include <sys/msg.h>


//FUNKCJE OBSŁUGI SYGNAŁÓW KAŻDEGO Z PROCESÓW
void obs_sig0(int signo);
void obs_sig1(int signo);
void obs_sig2(int signo);
void obs_sig3(int signo);


//FUNKCJE DZIAŁANIA PROCESÓW
void proces1(char *nazwaPliku);
void proces2(void);
void proces3(void);

//ZMIENNE GLOBALNE
static int P1,P2,P3;  //Pid'y każdego z procesów
 
int P;	  
int pipe_12, pipe_23; //Pipes pomiędzy procesami 1-2 oraz 2-3

int konczenie_programu=1; //Zmienna mowiaca czy konczyc program

bool plomba_1=0,plomba_2=0,plomba_3=0; //Powiedzmy ze tego nie ma ;)

int wybor, koniec=1; //Zmienne pomocnicze

char nazwa_pliku[100]=""; //Nazwa pliku do wczytania


/****************************************GŁÓWNA FUNKCJA PROGRAMU********************************************/

int main(){

	int h=1;	//Wczytywanie znakow
	for (h=1;h<62;h++)
		signal(h, obs_sig0);


	while(konczenie_programu){ //Pętla glownego programu
//-----------------------------------------------------------------------------------------------------------
//Czesc odpowiedzialna za komunikacje z uzytkownikiem odnosnie wprowadzanycxh danych
		printf("Wybierz zrodlo wczytywania danych: \n0 - stdin\n1 - plik\nWybieram: ");
		scanf("%d",&wybor);
		if(wybor==0){
			FILE *create_file;
			if ((create_file=fopen("temp.file", "w"))==NULL) {
				printf ("Nie moge otworzyc tymczasowego pliku\n");
				exit(1);
			}
			std::stack < char > stosZnakow;
			char znak;
			printf("Wpisuj liczby po kolei. Aby zakonczyc wpisywanie wpisz 0\n");
			while(znak=getchar()){
				if(znak==0)
					break;
				stosZnakow.push(znak);
			}
			fclose (create_file);
		}
		else if(wybor==1){
			printf("Podaj nazwe pliku: ");
			scanf("%s",nazwa_pliku);
		}
		else{
			printf("Nie ma takiej opcji");
		}
//-----------------------------------------------------------------------------------------------------------
//Tworzenie procesow
		if((P1=fork())==0){
			P1=getpid();
			if(wybor==0)
				proces1("temp.file");
			else if(wybor==1)
				proces1(nazwa_pliku);
		}else if(P1<0){
			perror("fork failed");
			exit(2);
		}
		
		if((P2=fork())==0){
			P2=getpid();
			proces2();
		}else if(P2<0){
			perror("fork failed");
			exit(2);
		}

		if((P3=fork())==0){
			P3=getpid();
			proces3();
		}else if(P3<0){
			perror("fork failed");
			exit(2);	
		}
		koniec=0;
		wait(NULL);
		wait(NULL);
		wait(NULL);
		koniec=1;
   
}
		
return 0;

}/*KONIEC GŁÓWNEJ FUNKCJI PROGRAMU*/


/*************************************FUNKCJE ZACHOWAŃ PROCESÓW*****************************************/
//-----------------------------------------------------------------------------------------------------------
//Proces 1 do wczytywania danych z pliku
void proces1(char *nazwaPliku){

	//Sygnaly ignorowane i lapanie sygnalow
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	int h=1;
	for (h=1;h<62;h++)
		signal(h, obs_sig1);

	//Tworzenie pipe'ow do komunikacji pomiedzy porcesami
	if (mkfifo ("fifo_12",0600) == -1)
	{
		perror("pipe failed");
		exit(1);
	}

	pipe_12=open("fifo_12", O_WRONLY);
	char napis[100];

	FILE *open_file;
	if ((open_file=fopen(nazwaPliku, "r"))==NULL) {
		printf ("Nie moge otworzyc pliku input.txt\n");
		exit(1);
	}

	printf("\n<Działanie procesu 1>\n");
	printf("<Trwa pobieranie danych z pliku i wysyłanie do procesu 2>\n");

	//wysykanie danych
	while(fgets (napis, 100, open_file))
	{
		write(pipe_12, napis, 100);
	}
	fclose (open_file);

	close(pipe_12);

	printf("\n---Proces 1 zakończył działanie---\n");

	exit(0);


}
//-----------------------------------------------------------------------------------------------------------
//Proces drugi odpowiedzialny za zliczanie ilosci znakow danych jakie zostaly mu dostarczone przez pipe

void proces2(void){
	
	//Sygnaly ignorowane i lapanie sygnalow
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	int h=1;
	for (h=1;h<62;h++)
		signal(h, obs_sig2);
	
	//Tworzenie pipe'ow do komunikacji pomiedzy porcesami
	if (mkfifo("fifo_23",0600) == -1)
	{
		perror("pipe failed");
		exit(1);
	}
	printf("\n<Działanie procesu 2>\n");

	//Odbieranie i wysylanie danych przez pipe
	char from_1_node[100],to_3_node[10];
	int i,size;
	pipe_23=open("fifo_23", O_WRONLY);
	pipe_12=open("fifo_12", O_RDONLY);
	while(read(pipe_12, from_1_node, sizeof(from_1_node))>0){ 
		size=0;
		for(i=0;from_1_node[i]!='\n';size++,i++);
		sprintf(to_3_node, "%d", size);
		write(pipe_23, to_3_node, 10);
	}
	close(pipe_23);
	close(pipe_12);
	unlink("fifo_12");
				


printf("\n---Proces 2 zakończył działanie---\n");
exit(0);
}

//-----------------------------------------------------------------------------------------------------------
//Proces odpowiedzialny za wyswielanie wyniku zliczania 
void proces3(void){

	//Sygnaly ignorowane i lapanie sygnalow
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	int h=1;
	for (h=1;h<62;h++)
		signal(h, obs_sig3);

	printf("\n<Działanie procesu 3>");

	//Odczytywanie i wyswielanie wyniku
	char from_2_node[10];
	pipe_23=open("fifo_23", O_RDONLY);
	while(read(pipe_23, from_2_node, sizeof(from_2_node))>0){ 
		printf("Suma: %s\n",from_2_node);
	}
	close(pipe_23);
	unlink("fifo_23");


printf("\n---Proces 3 zakończył działanie---\n");

printf("\n---Zakończenie działania programu---\n");
exit(0);

}


/****************************************OBSŁUGA SYGNAŁÓW***********************************************/

void obs_sig0(int signo){
	//Sygnal konczenia programu
	if(signo==SIGQUIT){
		if(koniec==0){
			printf("\nCzekanie na zakoczenie programu\n");
			wait(NULL);
			wait(NULL);	
			wait(NULL);
			konczenie_programu=0;
			unlink("fifo_12");
			unlink("fifo_23");
		}else if(koniec==1)
			exit(0);
		
	}
	//Sygnal wstrzymania procesow
	if(signo == SIGINT){
		printf("Wstrzymanie procesow!\n");
		plomba_1=0;
		plomba_2=0;
		plomba_3=0;
		kill(P1,SIGUSR1);
	}
	
	//Sygnal wznowienia procesow
	if(signo == SIGTSTP){
		printf("Wznowienie działania procesow!\n");
		plomba_1=1;
		plomba_2=1;
		plomba_3=1;
		kill(P1,SIGCONT);
	}
	
}

void obs_sig1(int signo){
	
	//Sygnal uspienia
	if(signo == SIGUSR1){
		if(!plomba_1){
			printf("\nProces 1 usypia\n");
			kill(P2,SIGUSR1);
			kill(P1,SIGSTOP);
			plomba_1=1;
			plomba_2=1;
			plomba_3=1;
		}
	}
	//Sygnal wznowienia
	else if(signo == SIGCONT){
		if(plomba_1){
			printf("\nProces 1 wstaje\n");
			kill(P2,SIGCONT);
			kill(P2,SIGCONT);
			plomba_1=0;
		}
	}
	else if(signo == SIGSTOP){
		;
	}
	
}
void obs_sig2(int signo){
	
	//Sygnal uspienia
	if(signo == SIGUSR1){
		if(!plomba_2){
			printf("\nProces 2 usypia\n");
			kill(P3,SIGUSR1);
			kill(P2,SIGSTOP);
			plomba_2=1;
		}
	}
	//sygnal wzowienia
	else if(signo == SIGCONT){
			if(plomba_2){
				printf("\nProces 2 wstaje\n");
				kill(P3,SIGCONT);
				kill(P3,SIGCONT);
				plomba_2=0;
			}
	}
	else if(signo == SIGSTOP){
		;
	}
	
}
void obs_sig3(int signo){
	
	//Sygnal uspienia
	if(signo == SIGUSR1){
		if(!plomba_3){
			printf("\nProces 3 usypia\n");
			kill(P3,SIGSTOP);
			plomba_3=1;
		}
	}
	//Syganl wznowienia
	else if(signo == SIGCONT){

		if(plomba_3){
			printf("\nProces 3 wstaje\n");
			plomba_3=0;
		}
	}
	else if(signo == SIGSTOP){
		;
	}
	
}
