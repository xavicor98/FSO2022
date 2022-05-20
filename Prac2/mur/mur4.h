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


#define MAX_LEN_ARG_STR 24
#define RETARD_DEFAULT 20
//#define RETARD_DEFAULT 100

enum ArgType {
	ARG_INDEX,
	ARG_ID_WIN,
	ARG_F,
	ARG_C,
	ARG_RETARD,
	ARG_FI1,
	ARG_FI2,
	ARG_NBALLS,
	ARG_NPROCS,
	ARG_NBLOCS,

	ARG_M_PAL,
	ARG_C_PAL,

	ARG_SEM_WIN,
	ARG_SEM_PAL,
	ARG_SEM_ARGS,

	ARGTYPE_COUNT,
};

enum ArgInfoPil {
	OFFSET_PIL_POS_F,
	OFFSET_PIL_POS_C,
	OFFSET_PIL_VEL_F,
	OFFSET_PIL_VEL_C,
	OFFSET_PIDS,
	ARGINFO_COUNT,
};
