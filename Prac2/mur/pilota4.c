#include <stdio.h> // incloure definicions de funcions estandard
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "memoria.h"
#include "winsuport2.h" // incloure definicions de funcions propies

#include "mur4.h"
#include "semafor.h"
#include "missatge.h"


//#include <pthread.h> // per crear threads
//#include <time.h> // mostrar temps de joc

/* definicio de constants */
/* variables globals */
char strin[LONGMISS]; /* variable per a generar missatges de text */
int retard; /* valor del retard de moviment, en mil.lisegons */


/*-----------------------------Variables globals per MUR3 >:D---------------------------------------------------------------------------------------------------------*/
int id_args;
float * args;
FILE * fd_debug;

int *fi1, *fi2;

pid_t * pids;
int * n_procs_pilota;

int n_fil, n_col; /* numero de files i columnes del taulell */
//int m_por; /* mida de la porteria (en caracters) */

int f_pal; //unused, usa el caracter '0' para detectar si rebota con la paleta...
int * ptr_c_pal; /* posicio del primer caracter de la paleta */
int m_pal; /* mida de la paleta */



/* vars pilota? vectors de >:C    fase 1.5*/
int f_pil, c_pil; /* posicio de la pilota, en valor enter */
float *pos_f, *pos_c; /* posicio de la pilota, en valor real */
float *vel_f, *vel_c; /* velocitat de la pilota, en valor real */
int nballs; /* num. pilotas carregar en carrega_configuracio... */
int *nblocs;
//void mou_pilota(int index);


/*-----------------------------Variables globals per MUR4 >:D---------------------------------------------------------------------------------------------------------*/
int id_sem_win;
int id_sem_pal;
int id_sem_args;

/* Si hi ha una col.lisió pilota-bloci esborra el bloc */
/* I genera nova pil */
void comprovar_bloc(int f, int c) {
	int col;

	char quin = win_quincar(f, c);
	if (quin == BLKCHAR || quin == FRNTCHAR) {
		col = c;
		while (win_quincar(f, col) != ' ') {
			win_escricar(f, col, ' ', NO_INV);
			col++;
		}
		col = c - 1;
		while (win_quincar(f, col) != ' ') {
			win_escricar(f, col, ' ', NO_INV);
			col--;
		}

		waitS(id_sem_args); // que nadie toque args, voy a leer n_procs y pasar args
		if ((quin == BLKCHAR) && (*n_procs_pilota < nballs)) {
			int _ = *n_procs_pilota; //TDDO temporal, da palo poner *n_procs_pilota
			pids[_] = fork();
			if (pids[_] == (pid_t) 0) {
				fprintf(fd_debug, "DEBUG: fork() desde #%d\n", _);
				args[ARG_INDEX] = _;
				//args[ARG_ID_WIN] =  id_win;
				//args[ARG_RETARD] = retard;
				//args[ARG_F] =  n_fil;
				//args[ARG_C] = n_col;
				//args[ARG_NPROCS] =_;
				//args[ARG_NBALLS] = *nballs;
				//args[ARG_NBLOCS] = nblocs;
				
				// pilota4 quiere saber las pos&vel de las pil del config para saber DONDE crear nuevas
				//memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_POS_F*(*nballs)], pos_f, sizeof(float)*(*nballs));
				//memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_POS_C*(*nballs)], pos_c, sizeof(float)*(*nballs));
				//memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_VEL_F*(*nballs)], vel_f, sizeof(float)*(*nballs));
				//memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_VEL_C*(*nballs)], vel_c, sizeof(float)*(*nballs));
				//memcpy(&args[ARGTYPE_COUNT+OFFSET_PIDS*(*nballs)], pids, sizeof(float)*(*nballs));
				char sz_id_args[MAX_LEN_ARG_STR];
				snprintf(sz_id_args, MAX_LEN_ARG_STR, "%i", id_args); // para pasar id_args a ./pilota4 por string
				execlp("./pilota4", "pilota4", sz_id_args, (char*) NULL);
				fprintf(stderr, "DEBUG: no puc executar el proc pilota4\n");
				fprintf(fd_debug, "Error: no s'ha pogut crear el proc pilota #%d.\n", *n_procs_pilota);
				fclose(fd_debug);
				exit(1);
			} else if (pids[_] > 0) {
				fprintf(fd_debug, "DEBUG: crear proc pilota #%d@\n", *n_procs_pilota);
				(*n_procs_pilota)++;
			} else {
				fprintf(stderr, "Error: no s'ha pogut crear el proc pilota #%d.\n", *n_procs_pilota);
				fprintf(fd_debug, "Error: no s'ha pogut crear el proc pilota #%d.\n", *n_procs_pilota);
				signalS(id_sem_args);
			}
		} else {
			signalS(id_sem_args);
		}
		(*nblocs)--;
	} else {
	}
}


//MAGIA
float control_impacte2(int c_pil, float velc0) {
	int distApal;
	float vel_c;
	waitS(id_sem_pal);
	distApal = c_pil - *ptr_c_pal;
	if (distApal >= 2*m_pal/3) { // costat dreta
		vel_c = 0.5;
	} else if (distApal <= m_pal/3) { // costat esquerra
		vel_c = -0.5;
	} else if (distApal == m_pal/2) { // al centre
		vel_c = 0.0;
	} else { // rebot normal
		vel_c = velc0;
	}
	signalS(id_sem_pal);
	return vel_c;
}

int mou_pilota(int index) {
	int f_h, c_h;
	char rh, rv, rd;
	int fora = 0;
	f_h = pos_f[index] + vel_f[index]; // posicio hipotetica de la pilota (entera)
	c_h = pos_c[index] + vel_c[index];
	rh = rv = rd = ' ';
	if ((f_h != f_pil) || (c_h != c_pil)) { // si posicio hipotetica no coincideix amb la posicio actual
		waitS(id_sem_win); // que nadie toque el tablero!
		if (f_h != f_pil) { // provar rebot vertical
			rv = win_quincar(f_h, c_pil); // veure si hi ha algun obstacle
			if (rv != ' ') { // si hi ha alguna cosa
				comprovar_bloc(f_h, c_pil);
				if (rv == '0') { // col.lisió amb la paleta?
					vel_c[index] = control_impacte2(c_pil, vel_c[index]);
				}
				vel_f[index] = -vel_f[index]; // canvia sentit velocitat vertical
				f_h = pos_f[index] + vel_f[index]; // actualitza posicio hipotetica
			}
		}
		if (c_h != c_pil) { // provar rebot horitzontal
			rh = win_quincar(f_pil, c_h); // veure si hi ha algun obstacle
			if (rh != ' ') { // si hi ha algun obstacle
				comprovar_bloc(f_pil, c_h);
				vel_c[index] = -vel_c[index]; // canvia sentit vel. horitzontal
				c_h = pos_c[index] + vel_c[index]; // actualitza posicio hipotetica
			}
		}
		if ((f_h != f_pil) && (c_h != c_pil)) { // provar rebot diagonal
			rd = win_quincar(f_h, c_h);
			if (rd != ' ') { // si hi ha obstacle
				comprovar_bloc(f_h, c_h);
				vel_f[index] = -vel_f[index];
				vel_c[index] = -vel_c[index]; // canvia sentit velocitats
				f_h = pos_f[index] + vel_f[index];
				c_h = pos_c[index] + vel_c[index]; // actualitza posicio entera
			}
		}
		// mostrar la pilota a la nova posició & verificar posicio definitiva
		if (win_quincar(f_h, c_h) == ' ') { // si no hi ha obstacle
			fprintf(fd_debug, "DEBUG: [pilota4.c] it's alive pil[%d] @ f=%f, c=%f \n\n", index, pos_f[index], pos_c[index]);
			win_escricar(f_pil, c_pil, ' ', NO_INV); // esborra pilota
			pos_f[index] += vel_f[index];
			pos_c[index] += vel_c[index];
			f_pil = f_h;
			c_pil = c_h; // actualitza posicio actual
			if (f_pil != n_fil - 1) { // si no surt del taulell,
				win_escricar(f_pil, c_pil, '1'+index, INVERS); // imprimeix pilota
			} else {
				fora = 1;
			}
		}
		signalS(id_sem_win);
	} else { // posicio hipotetica = a la real: moure
		pos_f[index] += vel_f[index];
		pos_c[index] += vel_c[index];
	}
	return (*nblocs==0 || fora); // TODO Tiene que acabar el juego cuando sale UNA pelota, o cuando han salido TODAS?
}

/* programa principal */
int main(int n_args, char *ll_args[]) {
	fd_debug = fopen("error_log_pil.txt", "wa");

	fprintf(fd_debug, "DEBUG: [pilota4.c] A star is born...\n");
	void * ptr_win;
	id_args = atoi(ll_args[1]);
	args = map_mem(id_args);
	int index = args[ARG_INDEX];
	int id_win = args[ARG_ID_WIN];
	retard = args[ARG_RETARD];
	n_fil = args[ARG_F];
	n_col = args[ARG_C];
	fi1 = (int*)&args[ARG_FI1];
	fi2 = (int*)&args[ARG_FI2];
	nballs = args[ARG_NBALLS];
	nblocs = (int*) &args[ARG_NBLOCS];
	n_procs_pilota = (int*) &args[ARG_NPROCS];
	
	m_pal = args[ARG_M_PAL];
	ptr_c_pal = (int*) &args[ARG_C_PAL];

	id_sem_win = args[ARG_SEM_WIN];
	id_sem_pal = args[ARG_SEM_PAL];
	id_sem_args = args[ARG_SEM_ARGS];
	
	pos_f = &args[ARGTYPE_COUNT+OFFSET_PIL_POS_F*nballs];
	pos_c = &args[ARGTYPE_COUNT+OFFSET_PIL_POS_C*nballs];
	vel_f = &args[ARGTYPE_COUNT+OFFSET_PIL_VEL_F*nballs];
	vel_c = &args[ARGTYPE_COUNT+OFFSET_PIL_VEL_C*nballs];
	pids = (pid_t*) &args[ARGTYPE_COUNT+OFFSET_PIDS*nballs];

	f_pil = pos_f[index];
	c_pil = pos_c[index];
	fprintf(fd_debug, "DEBUG: [pilota4.c] Hello world desde pil[%d]\n", index);

	for (int i = 0; i < nballs; i++) {
		fprintf(fd_debug, "DEBUG: [pilota4.c] pasa pil[%d] pf=%f, pc=%f vf=%f vc=%f\n\n", i, pos_f[i], pos_c[i], vel_f[i], vel_c[i]);
	}

	signalS(id_sem_args); // indicar que he acabado de leer todos los args

	ptr_win = map_mem(id_win);
	if (ptr_win == (int*) -1) {
		fprintf(fd_debug, "Error: [pilota4.c] Proc #%d error en identificador de finestra...\n", (int) getpid());
		fclose(fd_debug);
		exit(1);
	}
	if (fi1 == (int*) -1) {
		fprintf(fd_debug, "Error: [pilota4.c] Proc #%d error en identificador de fi1...\n", (int) getpid());
		fclose(fd_debug);
		exit(1);
	}
	if (fi2 == (int*) -1) {
		fprintf(fd_debug, "Error: [pilota4.c] Proc #%d error en identificador de fi2...\n", (int) getpid());
		fclose(fd_debug);
		exit(1);
	}
	win_set(ptr_win, n_fil, n_col);
	do {
		*fi2 = mou_pilota(index);
		win_retard(retard); // retard del joc
	} while (!(*fi1) && !(*fi2));
	fprintf(fd_debug, "Error: [pilota4.c] Proc #%d (pil #%d) dice ADEU!\n", (int) getpid(), index);
	fclose(fd_debug);
	return (0); //TODO retornar algo?
}
