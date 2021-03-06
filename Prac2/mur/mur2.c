/*****************************************************************************/
/*                                                                           */
/*                           mur0.c                                          */
/*                                                                           */
/*  Programa inicial d'exemple per a les practiques 2 de FSO.                */
/*                                                                           */
/*  Compilar i executar:                                                     */
/*     El programa invoca les funcions definides a "winsuport.c", les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':                                   */
/*                                                                           */
/*       $ gcc -Wall -c winsuport.c -o winsuport.o                           */
/*       $ gcc -Wall mur0.c winsuport.o -o mur0 -lcurses                     */
/*                                                                           */
/*  Al tenir una orientaci√≥ vertical cal tenir un terminal amb prou files.   */
/*  Per exemple:                                                             */
/*               xterm -geometry 80x50                                       */
/*               gnome-terminal --geometry 80x50                             */
/*                                                                           */
/*****************************************************************************/

/* #include <stdint.h> //intptr_t for 64bits machines */

#include <stdio.h> // incloure definicions de funcions estandard
#include <stdlib.h>
#include <string.h>
#include "winsuport.h" // incloure definicions de funcions propies


#include <pthread.h> // per crear threads
#include <time.h> // mostrar temps de joc

/* definicio de constants */
#define MAX_THREADS 10
#define MAXBALLS (MAX_THREADS-1)
#define MIN_FIL 10 /* dimensions del camp. Depenen del terminal! */
#define MAX_FIL 50
#define MIN_COL 10
#define MAX_COL 80
#define BLKSIZE 3
#define BLKGAP 2
#define BLKCHAR 'B'
#define WLLCHAR '#'
#define FRNTCHAR 'A'
#define LONGMISS 65

/* variables globals */
char *descripcio[] = {
	"\n",
	"Aquest programa implementa una versio basica del joc Arkanoid o Breakout:\n",
	"generar un camp de joc rectangular amb una porteria, una paleta que s\'ha\n",
	"de moure amb el teclat per a cobrir la porteria, i una pilota que rebota\n",
	"contra les parets del camp, a la paleta i els blocs. El programa acaba si\n",
	"la pilota surt per la porteria o no queden mes blocs. Tambe es pot acabar\n",
	"amb la tecla RETURN.\n",
	"\n",
	"  Arguments del programa:\n",
	"\n",
	"       $ ./mur0 fitxer_config [retard]\n",
	"\n",
	"     El primer argument ha de ser el nom d\'un fitxer de text amb la\n",
	"     configuracio de la partida, on la primera fila inclou informacio\n",
	"     del camp de joc (valors enters), i la segona fila indica posicio\n",
	"     i velocitat de la pilota (valors reals):\n",
	"          num_files  num_columnes  mida_porteria\n",
	"          pos_fil_pal pos_col_pal mida_pal\n",
	"          pos_fila   pos_columna   vel_fila  vel_columna\n",
	"\n",
	"     on els valors minims i maxims admesos son els seguents:\n",
	"          MIN_FIL <= num_files     <= MAX_FIL\n",
	"          MIN_COL <= num_columnes  <= MAX_COL\n",
	"          0 < mida_porteria < num_files-2\n",
	"        1.0 <= pos_fila     <= num_files-3\n",
	"        1.0 <= pos_columna  <= num_columnes\n",
	"       -1.0 <= vel_fila     <= 1.0\n",
	"       -1.0 <= vel_columna  <= 1.0\n",
	"\n",
	"     Alternativament, es pot donar el valor 0 a num_files i num_columnes\n",
	"     per especificar que es vol que el tauler ocupi tota la pantalla. Si\n",
	"     tambe fixem mida_porteria a 0, el programa ajustara la mida d\'aquesta\n",
	"     a 3/4 de l\'altura del camp de joc.\n",
	"\n",
	"     A mes, es pot afegir un segon argument opcional per indicar el\n",
	"     retard de moviment del joc en mil.lisegons; el valor minim es 10,\n",
	"     el valor maxim es 1000, i el valor per defecte d'aquest parametre\n",
	"     es 100 (1 decima de segon).\n",
	"\n",
	"  Codis de retorn:\n",
	"     El programa retorna algun dels seguents codis:\n",
	"	0  ==>  funcionament normal\n",
	"	1  ==>  numero d'arguments incorrecte\n",
	"	2  ==>  no s\'ha pogut obrir el fitxer de configuracio\n",
	"	3  ==>  algun parametre del fitxer de configuracio es erroni\n",
	"	4  ==>  no s\'ha pogut crear el camp de joc (no pot iniciar CURSES)\n",
	"\n",
	"   Per a que pugui funcionar aquest programa cal tenir instal.lada la\n",
	"   llibreria de CURSES (qualsevol versio).\n",
	"\n",
	"*"
};

int n_fil, n_col; /* numero de files i columnes del taulell */
int m_por; /* mida de la porteria (en caracters) */
int f_pal, c_pal; /* posicio del primer caracter de la paleta */
int m_pal; /* mida de la paleta */



int nblocs = 0;
int dirPaleta = 0;
int retard; /* valor del retard de moviment, en mil.lisegons */

char strin[LONGMISS]; /* variable per a generar missatges de text */

/*-----------------------------Variables globals per MUR1 >:D---------------------------------------------------------------------------------------------------------*/
int fi1, fi2;

/* vars a vectors de >:C    fase 1.5*/
int f_pil[MAXBALLS], c_pil[MAXBALLS]; /* posicio de la pilota, en valor enter */
float pos_f[MAXBALLS], pos_c[MAXBALLS]; /* posicio de la pilota, en valor real */
float vel_f[MAXBALLS], vel_c[MAXBALLS]; /* velocitat de la pilota, en valor real */
int nballs; /* num. pilotas carregar en carrega_configuracio... */

pthread_t threads_pilota[MAXBALLS];
int n_threads_pilota;
void * mou_pilota(void * index);

/*-----------------------------Variables globals per MUR2 >:D---------------------------------------------------------------------------------------------------------*/
pthread_mutex_t mutex_win = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pal = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pil = PTHREAD_MUTEX_INITIALIZER;

/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE * fit) {
	int ret = 0;

	fscanf(fit, "%d %d %d\n", &n_fil, &n_col, &m_por); // camp de joc
	fscanf(fit, "%d %d %d\n", &f_pal, &c_pal, &m_pal); // paleta
	//
	nballs = 0;
	do {
		fscanf(fit, "%f %f %f %f\n", &pos_f[nballs], &pos_c[nballs], &vel_f[nballs], &vel_c[nballs]); // pilota
		if ((n_fil != 0) || (n_col != 0)) { // si no dimensions maximes
			if ((n_fil < MIN_FIL) || (n_fil > MAX_FIL) || (n_col < MIN_COL) || (n_col > MAX_COL)) {
				ret = 1;
			} else if (m_por > n_col - 3) {
				ret = 2;
			} else if ((m_pal > n_col - 3) || (m_pal < 1) || (f_pal > n_fil-1) || (f_pal < 1) || (c_pal + m_pal > n_col -1 || c_pal < 1 )) {
				ret = 3;
			} else if ((pos_f[nballs] < 1) || (pos_f[nballs] >= n_fil - 3) || (pos_c[nballs] < 1) || (pos_c[nballs] > n_col - 1)) { // tres files especials: l√≠nia d'estat, porteria i paleta
				ret = 4;
			}
		}
		if ((vel_f[nballs] < -1.0) || (vel_f[nballs] > 1.0) || (vel_c[nballs] < -1.0) || (vel_c[nballs] > 1.0)) {
			ret = 5;
		}
		f_pil[nballs] = pos_f[nballs]; // fix que escribe ' '  en [0, 0] cuando crea una nueva pil
		c_pil[nballs] = pos_c[nballs];
		nballs++;
	} while (!feof(fit) && (nballs < MAXBALLS) && (ret == 0)); // carregar pilotes del fit
	fclose(fit);
	fprintf(stderr, "#DEBUG HE CARGADO %d pelotas del fitxer!\n", nballs); // TODO DEBUG

	if (ret != 0) { // si ha detectat algun error
		fprintf(stderr, "Error en fitxer de configuracio:\n");
		switch (ret) {
			case 1:
				fprintf(stderr, "\tdimensions del camp de joc incorrectes:\n");
				fprintf(stderr, "\tn_fil= %d \tn_col= %d\n", n_fil, n_col);
				break;
			case 2:
				fprintf(stderr, "\tmida de la porteria incorrecta:\n");
				fprintf(stderr, "\tm_por= %d\n", m_por);
				break;
			case 3:
				fprintf(stderr,"\tmida de la paleta incorrecta:\n");
				fprintf(stderr, "\tf_pal= %d \tc_pal= %d \t m_pal=%d\n", f_pal,c_pal,m_pal);
				break;
			case 4:
				fprintf(stderr, "\tposicio de la pilota #%d incorrecta:\n", nballs);
				fprintf(stderr, "\tpos_f= %.2f \tpos_c= %.2f\n", pos_f[nballs-1], pos_c[nballs-1]);
				break;
			case 5:
				fprintf(stderr, "\tvelocitat de la pilota #%d incorrecta:\n", nballs);
				fprintf(stderr, "\tvel_f= %.2f \tvel_c= %.2f\n", vel_f[nballs-1], vel_c[nballs-1]);
				break;
		}
	}
	return (ret);
}

/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
/* retorna diferent de zero si hi ha algun problema */
int inicialitza_joc(void) {
	int i, retwin;
	int i_port, f_port; // inici i final de porteria
	int c, nb, offset;

	retwin = win_ini(&n_fil, &n_col, '+', INVERS); // intenta crear taulell

	if (retwin < 0) { // si no pot crear l'entorn de joc amb les curses
		fprintf(stderr, "Error en la creacio del taulell de joc:\t");
		switch (retwin) {
			case -1:
				fprintf(stderr, "camp de joc ja creat!\n");
				break;
			case -2:
				fprintf(stderr, "no s'ha pogut inicialitzar l'entorn de curses!\n");
				break;
			case -3:
				fprintf(stderr, "les mides del camp demanades son massa grans!\n");
				break;
			case -4:
				fprintf(stderr, "no s'ha pogut crear la finestra!\n");
				break;
		}
		return (retwin);
	}

	if (m_por > n_col - 2) {
		m_por = n_col - 2; // limita valor de la porteria
	}
	if (m_por == 0) {
		m_por = 3 * (n_col - 2) / 4; // valor porteria per defecte
	}

	i_port = n_col / 2 - m_por / 2 - 1; // crea el forat de la porteria
	f_port = i_port + m_por - 1;
	for (i = i_port; i <= f_port; i++) {
		win_escricar(n_fil - 2, i, ' ', NO_INV);
	}

	n_fil = n_fil - 1; // descompta la fila de missatges

	for (i = 0; i < m_pal; i++) { // dibuixar paleta inicial
		win_escricar(f_pal, c_pal + i, '0', INVERS);
	}

	// generar la pilota
	if (pos_f[0] > n_fil - 1) {
		pos_f[0] = n_fil - 1; // limita posicio inicial de la pilota
	}
	if (pos_c[0] > n_col - 1) {
		pos_c[0] = n_col - 1;
	}
	f_pil[0] = pos_f[0];
	c_pil[0] = pos_c[0]; // dibuixar la pilota inicialment
	win_escricar(f_pil[0], c_pil[0], '1', INVERS);

	// generar els blocs
	nb = 0;
	nblocs = n_col / (BLKSIZE + BLKGAP) - 1;
	offset = (n_col - nblocs * (BLKSIZE + BLKGAP) + BLKGAP) / 2;
	for (i = 0; i < nblocs; i++) {
		for (c = 0; c < BLKSIZE; c++) {
			win_escricar(3, offset + c, FRNTCHAR, INVERS);
			nb++;
			win_escricar(4, offset + c, BLKCHAR, NO_INV);
			nb++;
			win_escricar(5, offset + c, FRNTCHAR, INVERS);
			nb++;
		}
		offset += BLKSIZE + BLKGAP;
	}
	nblocs = nb / BLKSIZE;
	// generar les defenses
	nb = n_col / (BLKSIZE + 2 * BLKGAP) - 2;
	offset = (n_col - nb * (BLKSIZE + 2 * BLKGAP) + BLKGAP) / 2;
	for (i = 0; i < nb; i++) {
		for (c = 0; c < BLKSIZE + BLKGAP; c++) {
			win_escricar(6, offset + c, WLLCHAR, NO_INV);
		}
		offset += BLKSIZE + 2 * BLKGAP;
	}

	sprintf(strin, "Tecles: \'%c\'-> Esquerra, \'%c\'-> Dreta, RETURN-> sortir\n", TEC_ESQUER, TEC_DRETA);
	win_escristr(strin);
	return (0);
}

/* funcio que escriu un missatge a la l√≠nia d'estat i tanca les curses */
void mostra_final(char *miss) {
	int lmarge;
	char marge[LONGMISS];

	// centrar el misssatge
	lmarge = (n_col+strlen(miss))/2;
	sprintf(marge,"%%%ds",lmarge);

	sprintf(strin, marge,miss);

	//TODO sec critica?
	win_escristr(strin);

	// espera tecla per a que es pugui veure el missatge
	getchar();
}

/* Si hi ha una col.lisi√≥ pilota-bloci esborra el bloc */
/* I genera nova pil */
void comprovar_bloc(int f, int c) {
	int col;

	pthread_mutex_lock(&mutex_win);
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
		pthread_mutex_unlock(&mutex_win);

		pthread_mutex_lock(&mutex_pil);
		if ((quin == BLKCHAR) && (n_threads_pilota < nballs)) {
			int tmp = n_threads_pilota;
			if (pthread_create(&threads_pilota[tmp], NULL, mou_pilota, &tmp) == 0) {
				n_threads_pilota++;
				//fprintf(stderr, "DEBUG: crea thread_pilota #%d @ comprovar_bloc\n", n_threads_pilota);
			} else {
				fprintf(stderr, "Error: no s'ha pogut crear el thread pilota #%d.\n", n_threads_pilota);
			}
				
		}
		pthread_mutex_unlock(&mutex_pil);
		nblocs--;
	} else {
		pthread_mutex_unlock(&mutex_win);
	}
}

/* funcio per a calcular rudimentariament els efectes amb la pala */
/* no te en compta si el moviment de la paleta no √©s recent */
/* cal tenir en compta que despr√©s es calcula el rebot */
void control_impacte(int i) {
	if (dirPaleta == TEC_DRETA) {
		if (vel_c[i] <= 0.0) { // pilota cap a l'esquerra
			vel_c[i] = -vel_c[i] - 0.2; // xoc: canvi de sentit i reduir velocitat
		} else { // a favor: incrementar velocitat
			if (vel_c[i] <= 0.8) {
				vel_c[i] += 0.2;
			}
		}
	} else {
		if (dirPaleta == TEC_ESQUER) {
			if (vel_c[i] >= 0.0) { // pilota cap a la dreta
				vel_c[i] = -vel_c[i] + 0.2; // xoc: canvi de sentit i reduir la velocitat
			} else { // a favor: incrementar velocitat
				if (vel_c[i] >= -0.8) {
					vel_c[i] -= 0.2;
				}
			}
		} else { // XXX trucs no documentats
			if (dirPaleta == TEC_AMUNT) {
				vel_c[i] = 0.0; // vertical
			} else {
				if (dirPaleta == TEC_AVALL) {
					if (vel_f[i] <= 1.0) {
						vel_f[i] -= 0.2; // frenar
					}
				}
			}
		}
	}
	dirPaleta = 0; // reset perque ja hem aplicat l'efecte
}


//MAGIA
float control_impacte2(int c_pil, float velc0) {
	int distApal;
	float vel_c;
	
	pthread_mutex_lock(&mutex_pal);
	distApal = c_pil - c_pal;
	pthread_mutex_unlock(&mutex_pal);
	if (distApal >= 2*m_pal/3) { // costat dreta
		vel_c = 0.5;
	} else if (distApal <= m_pal/3) { // costat esquerra
		vel_c = -0.5;
	} else if (distApal == m_pal/2) { // al centre
		vel_c = 0.0;
	} else { // rebot normal
		vel_c = velc0;
	}
	return vel_c;
}

/* funcio per moure la pilota: retorna un 1 si la pilota surt per la porteria,*/
/* altrament retorna un 0 */
void * mou_pilota(void * index) {
	int id = *(int*)index;
	int f_h, c_h;
	char rh, rv, rd;
	int fora = 0;
	do {
		f_h = pos_f[id] + vel_f[id]; // posicio hipotetica de la pilota (entera)
		c_h = pos_c[id] + vel_c[id];
		rh = rv = rd = ' ';
		if ((f_h != f_pil[id]) || (c_h != c_pil[id])) { // si posicio hipotetica no coincideix amb la posicio actual
			if (f_h != f_pil[id]) { // provar rebot vertical
				pthread_mutex_lock(&mutex_win);
				rv = win_quincar(f_h, c_pil[id]); // veure si hi ha algun obstacle
				if (rv != ' ') { // si hi ha alguna cosa
					pthread_mutex_unlock(&mutex_win);
					comprovar_bloc(f_h, c_pil[id]);
					if (rv == '0') { // col.lisi√≥ amb la paleta?
						// control_impacte(); ?
						// TODO la paleta se mueve desde otro thread, y dentro de control_impacte2()
						// se comprueva la pos de la paleta?
						vel_c[id] = control_impacte2(c_pil[id], vel_c[id]);
					}
					vel_f[id] = -vel_f[id]; // canvia sentit velocitat vertical
					f_h = pos_f[id] + vel_f[id]; // actualitza posicio hipotetica
				} else {
					pthread_mutex_unlock(&mutex_win);
				}
			}
			if (c_h != c_pil[id]) { // provar rebot horitzontal
				pthread_mutex_lock(&mutex_win);
				rh = win_quincar(f_pil[id], c_h); // veure si hi ha algun obstacle
				if (rh != ' ') { // si hi ha algun obstacle
					pthread_mutex_unlock(&mutex_win);
					comprovar_bloc(f_pil[id], c_h);
					// TODO?: tractar la col.lisio lateral amb la paleta
					vel_c[id] = -vel_c[id]; // canvia sentit vel. horitzontal
					c_h = pos_c[id] + vel_c[id]; // actualitza posicio hipotetica
				} else {
					pthread_mutex_unlock(&mutex_win);
				}
			}
			if ((f_h != f_pil[id]) && (c_h != c_pil[id])) { // provar rebot diagonal
				pthread_mutex_lock(&mutex_win);
				rd = win_quincar(f_h, c_h);
				if (rd != ' ') { // si hi ha obstacle
					pthread_mutex_unlock(&mutex_win);
					comprovar_bloc(f_h, c_h);
					vel_f[id] = -vel_f[id];
					vel_c[id] = -vel_c[id]; // canvia sentit velocitats
					f_h = pos_f[id] + vel_f[id];
					c_h = pos_c[id] + vel_c[id]; // actualitza posicio entera
				} else {
					pthread_mutex_unlock(&mutex_win);
				}
			}
			// mostrar la pilota a la nova posici√≥
			// verificar posicio definitiva
			pthread_mutex_lock(&mutex_win);
			if (win_quincar(f_h, c_h) == ' ') { // si no hi ha obstacle
				win_escricar(f_pil[id], c_pil[id], ' ', NO_INV); // esborra pilota
				pos_f[id] += vel_f[id];
				pos_c[id] += vel_c[id];
				f_pil[id] = f_h;
				c_pil[id] = c_h; // actualitza posicio actual
				if (f_pil[id] != n_fil - 1) { // si no surt del taulell,
					win_escricar(f_pil[id], c_pil[id], '1'+id, INVERS); // imprimeix pilota
				} else {
					fora = 1;
				}
			}
			pthread_mutex_unlock(&mutex_win);
		} else { // posicio hipotetica = a la real: moure
			pos_f[id] += vel_f[id];
			pos_c[id] += vel_c[id];
		}
		fi2 = (nblocs==0 || fora); // TODO Tiene que acabar el juego cuando sale UNA pelota, o cuando han salido TODAS?
		win_retard(retard); // retard del joc
	} while (!fi1 && !fi2);
	return NULL;
}

/* funcio per moure la paleta segons la tecla premuda */
/* retorna un boolea indicant si l'usuari vol acabar */
void * mou_paleta(void * nul) {
	int tecla, result;
	do {
		result = 0;
		pthread_mutex_lock(&mutex_win);
		tecla = win_gettec();
		pthread_mutex_unlock(&mutex_win);
		if (tecla != 0) {
			if ((tecla == TEC_DRETA) && ((c_pal + m_pal) < n_col - 1)) {
				pthread_mutex_lock(&mutex_win);
				win_escricar(f_pal, c_pal, ' ', NO_INV); // esborra primer bloc
				pthread_mutex_lock(&mutex_pal);
				c_pal++; // actualitza posicio
				pthread_mutex_unlock(&mutex_pal);
				win_escricar(f_pal, c_pal + m_pal - 1, '0', INVERS); //esc. ultim bloc
				pthread_mutex_unlock(&mutex_win);
			}
			if ((tecla == TEC_ESQUER) && (c_pal > 1)) {
				pthread_mutex_lock(&mutex_win);
				win_escricar(f_pal, c_pal + m_pal - 1, ' ', NO_INV); //esborra ultim bloc
				pthread_mutex_lock(&mutex_pal);
				c_pal--; // actualitza posicio
				pthread_mutex_unlock(&mutex_pal);
				win_escricar(f_pal, c_pal, '0', INVERS); // escriure primer bloc
				pthread_mutex_unlock(&mutex_win);
			}
			if (tecla == TEC_RETURN) {
				result = 1; // final per pulsacio RETURN
			}
			dirPaleta = tecla; // per a afectar al moviment de les pilotes
		}
		fi1 = (result);
	} while (!fi1 && !fi2);
	return NULL;
}

/* programa principal */
int main(int n_args, char *ll_args[]) {
	int i;
	FILE *fit_conf;

	pthread_t thread_paleta;
	n_threads_pilota = 0;
	long int t; // unused

	time_t start, ara;
	int delta, m, s;
	char strin[128];

	if ((n_args != 2) && (n_args != 3)) { // si numero d'arguments incorrecte
		i = 0;
		do {
			fprintf(stderr, "%s", descripcio[i++]); // imprimeix descripcio
		} while (descripcio[i][0] != '*'); // mentre no arribi al final
		exit(1);
	}

	fit_conf = fopen(ll_args[1], "rt"); // intenta obrir el fitxer
	if (!fit_conf) {
		fprintf(stderr, "Error: no s'ha pogut obrir el fitxer \'%s\'\n", ll_args[1]);
		exit(2);
	}

	if (carrega_configuracio(fit_conf) != 0) { // llegir dades del fitxer
		exit(3); // aborta si hi ha algun problema en el fitxer
	}

	if (n_args == 3) { // si s'ha especificat parametre de retard
		retard = atoi(ll_args[2]); // convertir-lo a enter
		if (retard < 10) retard = 10; // verificar limits
		if (retard > 1000) retard = 1000;
	} else {
		retard = 100; // altrament, fixar retard per defecte
	}

	printf("Joc del Mur: prem RETURN per continuar:\n");
	getchar();

	if (inicialitza_joc() != 0) { // intenta crear el taulell de joc
		exit(4); // aborta si hi ha algun problema amb taulell
	}

	int _ = pthread_mutex_init(&mutex_win, NULL);
	//fprintf(stderr, "DEBUG: pthread_mutex_init(mutex_win)  returns [%d]\n", _);
	_ = pthread_mutex_init(&mutex_pal, NULL);
	//fprintf(stderr, "DEBUG: pthread_mutex_init(mutex_pal)  returns [%d]\n", _);
	_ = pthread_mutex_init(&mutex_pil, NULL);
	//fprintf(stderr, "DEBUG: pthread_mutex_init(mutex_pil)  returns [%d]\n", _);
	
	//pthread_mutex_lock(&mutex_pil);
	_ = n_threads_pilota;
	if (pthread_create(&threads_pilota[0], NULL, mou_pilota, &_) == 0) {
		n_threads_pilota++;
		//fprintf(stderr, "DEBUG: crea thread_pilota \n");
	} else {
		fprintf(stderr, "Error: no s'ha pogut crear el thread pilota.\n");
	}
	//pthread_mutex_unlock(&mutex_pil);

	if (pthread_create(&thread_paleta, NULL, mou_paleta, (void *)NULL) == 0) {
		//fprintf(stderr, "DEBUG: crea thread_paleta.\n");
	} else {
		fprintf(stderr, "Error: no s'ha pogut crear el thread paleta \n");
	}

	time(&start);

	do {
		time(&ara);
		delta = difftime(ara, start);
		m = delta / 60;
		s = delta % 60;
		snprintf(strin, sizeof(strin), "%02d:%02d", m, s);
		pthread_mutex_lock(&mutex_win);
		win_escristr(strin);
		pthread_mutex_unlock(&mutex_win);

		win_retard(retard); // retard del joc
	} while (!fi1 && !fi2);


	for (i = 0; i < n_threads_pilota; i++) {
		pthread_join(threads_pilota[i], (void **)&t);
	}
	pthread_join(thread_paleta, (void **)&t);

	if (nblocs == 0) {
		mostra_final("YOU WIN !");
	} else {
		mostra_final("GAME OVER");
	}

	win_fi(); // tanca les curses

	pthread_mutex_destroy(&mutex_win);
	pthread_mutex_destroy(&mutex_pal);
	pthread_mutex_destroy(&mutex_pil);

	return (0); // retorna sense errors d'execucio
}
