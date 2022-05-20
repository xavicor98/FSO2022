#include <stdio.h> // incloure definicions de funcions estandard
#include <stdlib.h>



#include <pthread.h> // per crear threads
#include <sys/types.h>
#include <time.h> // mostrar temps de joc

#include <unistd.h> //fork()
#include <sys/wait.h> //waitpid()

#include <memory.h> //memcpy()  // se puede hacer un for() pero da palo
#include "winsuport2.h" // win_ini() &co.
#include "memoria.h" //ini_mem()  :D

#include "mur3.h" // defines & enums

int id_win;
void* ptr_win;
int mida_camp;

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
int * fi1, *fi2; //compartidas (pilota3 tiene que saber cuando acaba el juego, y tiene que poder decir cuando acaba el juego por fi2)

/* vars a vectors de >:C    fase 1.5*/
int f_pil[MAXBALLS], c_pil[MAXBALLS]; /* posicio de la pilota, en valor enter */
float pos_f[MAXBALLS], pos_c[MAXBALLS]; /* posicio de la pilota, en valor real */
float vel_f[MAXBALLS], vel_c[MAXBALLS]; /* velocitat de la pilota, en valor real */
int nballs; /* num. pilotas carregar en carrega_configuracio... */
// se pasan por param. a pilota3


/*-----------------------------Variables globals per MUR2 >:D---------------------------------------------------------------------------------------------------------*/
pthread_mutex_t mutex_win = PTHREAD_MUTEX_INITIALIZER; // lo dejamos para sincronizar entre thread paleta y main (no perdemos)
//pthread_mutex_t mutex_pal = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutex_pil = PTHREAD_MUTEX_INITIALIZER;

/*-----------------------------Variables globals per MUR3 >:D---------------------------------------------------------------------------------------------------------*/
pid_t * pids;
int * ptr_c_pal;

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
			} else if ((pos_f[nballs] < 1) || (pos_f[nballs] >= n_fil - 3) || (pos_c[nballs] < 1) || (pos_c[nballs] > n_col - 1)) { // tres files especials: línia d'estat, porteria i paleta
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
	int i;
	int i_port, f_port; // inici i final de porteria
	int c, nb, offset;

	mida_camp = win_ini(&n_fil, &n_col, '+', INVERS); // intenta crear taulell

	if (mida_camp < 0) { // si no pot crear l'entorn de joc amb les curses
		fprintf(stderr, "Error en la creacio del taulell de joc:\t");
		switch (mida_camp) {
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
		elim_mem(mida_camp);
		exit(2);
	}
	id_win = ini_mem(mida_camp);
	ptr_win = map_mem(id_win);
	win_set(ptr_win, n_fil, n_col);


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
	
	/*
	// generar la pilota (Dejamos que lo haga en/desde su propio proc...)
	if (pos_f[0] > n_fil - 1) {
		pos_f[0] = n_fil - 1; // limita posicio inicial de la pilota
	}
	if (pos_c[0] > n_col - 1) {
		pos_c[0] = n_col - 1;
	}
	f_pil[0] = pos_f[0];
	c_pil[0] = pos_c[0]; // dibuixar la pilota inicialment
	win_escricar(f_pil[0], c_pil[0], '1', INVERS);
	*/

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
	*fi1 = 0;
	*fi2 = 0;

	sprintf(strin, "Tecles: \'%c\'-> Esquerra, \'%c\'-> Dreta, RETURN-> sortir\n", TEC_ESQUER, TEC_DRETA);
	win_escristr(strin);
	return (0);
}

/* funcio que escriu un missatge a la línia d'estat i tanca les curses */
void mostra_final(char *miss) {
	int lmarge;
	char marge[LONGMISS];

	// centrar el misssatge
	lmarge = (n_col+strlen(miss))/2;
	sprintf(marge,"%%%ds",lmarge);

	sprintf(strin, marge,miss);

	//TODO sec critica?
	win_escristr(strin);

	win_update(); // enseñar msg y esperar
	// espera tecla per a que es pugui veure el missatge
	getchar();
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
			if ((tecla == TEC_DRETA) && (((*ptr_c_pal) + m_pal) < n_col - 1)) {
				pthread_mutex_lock(&mutex_win);
				win_escricar(f_pal, (*ptr_c_pal), ' ', NO_INV); // esborra primer bloc
				//pthread_mutex_lock(&mutex_pal);
				(*ptr_c_pal)++; // actualitza posicio
				//pthread_mutex_unlock(&mutex_pal);
				win_escricar(f_pal, (*ptr_c_pal) + m_pal - 1, '0', INVERS); //esc. ultim bloc
				pthread_mutex_unlock(&mutex_win);
			}
			if ((tecla == TEC_ESQUER) && ((*ptr_c_pal) > 1)) {
				pthread_mutex_lock(&mutex_win);
				win_escricar(f_pal, (*ptr_c_pal) + m_pal - 1, ' ', NO_INV); //esborra ultim bloc
				//pthread_mutex_lock(&mutex_pal);
				(*ptr_c_pal)--; // actualitza posicio
				//pthread_mutex_unlock(&mutex_pal);
				win_escricar(f_pal, (*ptr_c_pal), '0', INVERS); // escriure primer bloc
				pthread_mutex_unlock(&mutex_win);
			}
			//if (tecla == TEC_RETURN) {
			if (tecla == 'q') {
				result = 1; // final per pulsacio RETURN
			}
			dirPaleta = tecla; // per a afectar al moviment de les pilotes
		}
		*fi1 = (result);
	} while (!(*fi1) && !(*fi2));
	return NULL;
}

/* programa principal */
int main(int n_args, char *ll_args[]) {
	int i;
	FILE *fit_conf;

	pthread_t thread_paleta;
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
		retard = RETARD_DEFAULT; // altrament, fixar retard per defecte
	}
	//TODO PROCS + (considerar si esto deberia estar dento de inicialitza_joc?
	int id_args = ini_mem(sizeof(float) * (ARGTYPE_COUNT+(ARGINFO_COUNT*nballs)));
	float * args = map_mem(id_args); // guardar todas las variables compartidas en mem compartida // TODO ser un (void*)?
	char sz_id_args[MAX_LEN_ARG_STR];
	snprintf(sz_id_args, MAX_LEN_ARG_STR, "%i", id_args); // para pasar id_args a ./pilota3 por string

	fi1 = (int*) &args[ARG_FI1];
	fi2 = (int*) &args[ARG_FI2];
	int *n_procs_pilota = (int*) &args[ARG_NPROCS];
	ptr_c_pal = (int*) &args[ARG_C_PAL];
	*fi1 = 0;
	*fi2 = 0;
	*n_procs_pilota = 0;
	*ptr_c_pal = c_pal;
	pids = (pid_t*) &args[ARGTYPE_COUNT+OFFSET_PIDS*nballs];

	printf("Joc del Mur: prem RETURN per continuar:\n");
	//getchar();

	if (inicialitza_joc()) { // intenta crear el taulell de joc
		exit(4); // aborta si hi ha algun problema amb taulell
	}

	int _ = pthread_mutex_init(&mutex_win, NULL);
	//fprintf(stderr, "DEBUG: pthread_mutex_init(mutex_win)  returns [%d]\n", _);
	//_ = pthread_mutex_init(&mutex_pal, NULL); // no me toques la paleta
	//fprintf(stderr, "DEBUG: pthread_mutex_init(mutex_pal)  returns [%d]\n", _);

	//_ = pthread_mutex_init(&mutex_pil, NULL); //TOOD desde ahora se sincroniza con semaforos
	//fprintf(stderr, "DEBUG: pthread_mutex_init(mutex_pil)  returns [%d]\n", _);

	
	win_update(); // update debug despues de init

	//TOOD PROCS
	//pthread_mutex_lock(&mutex_pil);
	_ = *n_procs_pilota;
	args[ARG_INDEX] = _; //index de la pelota que se va a crear
	args[ARG_ID_WIN] =  id_win;
	args[ARG_RETARD] = retard;
	args[ARG_F] =  n_fil;
	args[ARG_C] = n_col;
	args[ARG_NBALLS] = nballs; //# pel cargadas en cfg
	args[ARG_NBLOCS] = nblocs; //# de blocs a que quedan por destruir
	args[ARG_M_PAL] = m_pal; //# width paleta
	//args[ARG_C_PAL] = c_pal; //DEBUG no tocar (??)
	
	// pilota3 quiere saber las pos&vel de las pil del config para saber DONDE crear nuevas
	memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_POS_F*nballs], pos_f, sizeof(float)*nballs);
	memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_POS_C*nballs], pos_c, sizeof(float)*nballs);
	memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_VEL_F*nballs], vel_f, sizeof(float)*nballs);
	memcpy(&args[ARGTYPE_COUNT+OFFSET_PIL_VEL_C*nballs], vel_c, sizeof(float)*nballs);
	memcpy(&args[ARGTYPE_COUNT+OFFSET_PIDS*nballs], pids, sizeof(pid_t)*nballs);

	pids[_] = fork();
	if (pids[_] == (pid_t) 0) {
		fprintf(stderr, "DEBUG: BRBRBRBBRBR pu\n");
		execlp("./pilota3", "pilota3", sz_id_args, (char*) NULL);
		fprintf(stderr, "DEBUG: no puc executar el proc pilota3\n");
		exit(1);
	} else if (pids[_] > 0) {
		(*n_procs_pilota)++;
	} else {
		fprintf(stderr, "Error: no s'ha pogut crear el proc de la primera pilota.\n");
	}
	//pthread_mutex_unlock(&mutex_pil);

	if (pthread_create(&thread_paleta, NULL, mou_paleta, (void *)NULL) == 0) {
		//fprintf(stderr, "DEBUG: crea thread_paleta.\n");
	} else {
		fprintf(stderr, "Error: no s'ha pogut crear el thread paleta \n");
	}

	time(&start);

	do {
		win_update();
		time(&ara);
		delta = difftime(ara, start);
		m = delta / 60;
		s = delta % 60;
		snprintf(strin, sizeof(strin), "%02d:%02d", m, s);
		pthread_mutex_lock(&mutex_win);
		win_escristr(strin);
		pthread_mutex_unlock(&mutex_win);
		win_retard(RETARD_DEFAULT); // cada ~100ms ?
	} while (!(*fi1) && !(*fi2));


	// esperar a los otros procs
	_ = 0;
	for (i = 0; i<(*n_procs_pilota); i++) {
		waitpid(pids[i], &_, 0);
		//_tmp >>= 8;
	}
	pthread_join(thread_paleta, (void **)&t);

	if (args[ARG_NBLOCS] < 1) {
		mostra_final("YOU WIN !");
	} else {
		mostra_final("GAME OVER");
	}

	win_fi(); // tanca les curses

	pthread_mutex_destroy(&mutex_win);
	//pthread_mutex_destroy(&mutex_pal);
	//pthread_mutex_destroy(&mutex_pil);

	elim_mem(id_args);
	elim_mem(id_win);

	return (0); // retorna sense errors d'execucio
}
