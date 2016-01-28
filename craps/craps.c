/**
 * Game of luck: Implementation of the Gamemaster
 *
 * Course: Operating Systems and Multicore Programming - OSM lab
 * assignment 1: game of luck.
 *
 * Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
 *
 */

#include <stdio.h> /* I/O functions: printf() ... */
#include <stdlib.h> /* rand(), srand() */
#include <unistd.h> /* read(), write() calls */
#include <assert.h> /* assert() */
#include <time.h>   /* time() */
#include <signal.h> /* kill(), raise() and SIG???? */
#include <stdbool.h> 
#include <sys/types.h> /* pid */
#include <sys/wait.h> /* waitpid() */

#include "common.h"

int main(int argc, char *argv[])
{
	int i, seed;
	pid_t children[NUM_PLAYERS];

        /* TODO: Use the following variables in the exec system call. Using the
	 * function sprintf and the arg1 variable you can pass the id parameter
	 * to the children
	 */

	char arg0[] = "./shooter";
	char arg1[10];
	char *args[] = {arg0, arg1, NULL};

	int pipe_seed[NUM_PLAYERS][2];
	int pipe_score[NUM_PLAYERS][2];
	
	for (i = 0; i < NUM_PLAYERS; i++) {
	  /* TODO: initialize the communication with the players */
	 pipe(pipe_seed[i]);
	 pipe(pipe_score[i]);
	}
	


	bool parent = true;
	for (i = 0; i < NUM_PLAYERS; i++) {
	  /* TODO: spawn the processes that simulate the players */
	  if (parent){
	    pid_t pid = fork();

	    switch(pid){

	    case -1: //error
       	      exit(EXIT_FAILURE);

	    case  0:  //child
	      parent = false;
	      /* Close unnecessary pipes */
	      for (int k = 0; k < NUM_PLAYERS; k++) {
		if(k != i) {
		  close((pipe_seed[k])[0]);
		  close((pipe_seed[k])[1]); //close all children read
		  close((pipe_score[k])[0]); //close all children write
		  close((pipe_score[k])[1]);
		}
		else if (k == 1) {
		  close((pipe_seed[k])[1]); //close write seed
		  close((pipe_score[k])[0]); //close read score
		}
	      }
	      sprintf(arg1, "%d", i);
	      dup2(pipe_seed[i][0], STDIN_FILENO);
	      dup2(pipe_score[i][1], STDOUT_FILENO);
	      //shooter(i, pipe_seed[i][0], pipe_score[i][1]);
	      execv(args[0], args);
	      break;
      
	    default:
	      children[i] = pid; //parent
	      break;
	    }
	  }
	}

	
	//seed = time(NULL); //Det är roligare med clock hihi
	for (i = 0; i < NUM_PLAYERS; i++) {
	  seed = clock(); 
	  //seed++; //detta är endast till för om man vill använda time(NULL)

	  /* TODO: send the seed to the players */
     	  write((pipe_seed[i])[1], &seed, sizeof(int));
	   
	}
	
	/* TODO: get the dice results from the players, find the winner */
	int highscore = 0;
	int winner = 0;
	for (i = 0; i < NUM_PLAYERS; i++) {
	  int current_score;
	  read(pipe_score[i][0], &current_score, sizeof(int));
	  if (current_score > highscore) {
	    highscore = current_score;
	    winner = i;
	  }
	}
	printf("\nmaster: player %d WINS\n", winner);
        kill(children[winner], SIGUSR1);
	/* TODO: signal the winner */

	printf("master: the game ends\n\n");
	/* TODO: signal all players the end of game */
	for (i = 0; i < NUM_PLAYERS; i++) {
	 kill(children[i], SIGUSR2);
	}


	
	/* TODO: cleanup resources and exit with success */
	for (i = 0; i < NUM_PLAYERS; i++) {
	  int waitstatus = wait(NULL);
     	  waitstatus = wait(&waitstatus);
	  exit(EXIT_SUCCESS);
	}
	return 0;
}
