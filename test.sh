#! /bin/bash


err() {
	# echo "[$(date +'%Y-%m-%dT%H:%M:%S%z')]: $*" >&2
	echo "ERROR: $*" >&2
}

usage() {
	echo "USAGE:"
	echo "${0}"
	echo "	[-h] Mostra help"
	echo "	[-n nomFit] Nom del fitxer on guardar la config."
	echo "	[-f numFiles] # Files de la zona (10..120)"
	echo "	[-c numColum] # Columnes de la zona (10..36)"
	echo "	[-p midaPort] Mida de la porteria (8..numFiles-1)"
	echo "	[-0 posFilaPal,posColPal] Fila i columna de la paleta (2..118),(2..35)"
	echo "	[-m midaPaleta] Mida de la paleta (3..numFiles-1)"
	echo "	[-1 posFilaPil,posColPil,velFil,velCol] Fila, columna, velocitat vertical i horitzontal de la pilota en joc (2..118),(2..35),(-1.0..1.0),(-1.0..1.0). Aquesta opt es pot repetir fins a 9 vegades."
}



FILENAME="" # Nom del fitxer on guardar la config
Y=0 # Files
X=0 # Cols
WIDTH_GOAL=0 # Mida porteria
Y_PADDLE=0 # Fila pal
X_PADDLE=0 # Col pal
WIDTH_PADDLE=0 # Mida pal
N=0 # Num. de pilotas
SERVES="" # Info posFilaPil posColPil velFil velCol de pilotas


LOAD_Y=0 # Files en fitxer
LOAD_X=0 # Cols en fitxer
LOAD_WIDTH_GOAL=0 # Mida porteria en fitxer

LOAD_Y_PADDLE=0 # Fila pal en fitxer
LOAD_X_PADDLE=0 # Col pal en fitxer
LOAD_WIDTH_PADDLE=0 # Mida pal en fitxer

LOAD_N=0 # Num. de pilotas en fitxer
LOAD_SERVES="" # Info pilotas en fitxer


CONFIG_Y=0 # Files a guardar
CONFIG_X=0 # Cols a guardar
CONFIG_WIDTH_GOAL=0 # Mida porteria a guardar

CONFIG_Y_PADDLE=0 # Fila pal a guardar
CONFIG_X_PADDLE=0 # Col pal a guardar
CONFIG_WIDTH_PADDLE=0 # Mida pal a guardar

CONFIG_N=0 # Num. de pilotas a guardar
CONFIG_SERVES="" # Info pilotas a guardar


load_config() {
	echo "loading config from \"$FILENAME\" :D"
	# Asumir que la config guardada en un fitxer sempre correcta:
	LINE_COUNT=0
	while read -r LINE; do
		case "${LINE_COUNT}" in
			0) # La primera: numFiles numColum midaPort
				read -r LOAD_Y LOAD_X LOAD_WIDTH_GOAL <<< "${LINE}"
				;;

			1) # La segona: posFilaPal posColPal midaPaleta
				read -r LOAD_Y_PADDLE LOAD_X_PADDLE LOAD_WIDTH_PADDLE <<< "${LINE}"
				;;

			*) # La tercera -- fins ultima: pilota en joc: posFila posCol velFil velCol
				read -r LOAD_BALL_Y LOAD_BALL_X LOAD_BALL_SPEED_Y LOAD_BALL_SPEED_X <<< "${LINE}"
				LOAD_SERVES="${LOAD_SERVES} ${LOAD_BALL_Y},${LOAD_BALL_X},${LOAD_BALL_SPEED_Y},${LOAD_BALL_SPEED_X}"
				LOAD_N=$((LOAD_N+1))
				;;

		esac
		LINE_COUNT=$((LINE_COUNT+1))
		echo "LINE=\"${LINE}\""
		echo "LINE_COUNT=${LINE_COUNT}"

	done < ${FILENAME}
	echo :: ${LOAD_Y}=LOAD_Y
	echo :: ${LOAD_X}=LOAD_X
	echo :: ${LOAD_WIDTH_GOAL}=LOAD_WIDTH_GOAL
	echo
	echo :: ${LOAD_Y_PADDLE}=LOAD_Y_PADDLE
	echo :: ${LOAD_X_PADDLE}=LOAD_X_PADDLE
	echo :: ${LOAD_WIDTH_PADDLE}=LOAD_WIDTH_PADDLE
	echo
	echo :: ${LOAD_N}=LOAD_N
	echo :: ${LOAD_SERVES}=LOAD_SERVES
	echo
}


save_config() {
	echo "${CONFIG_Y} ${CONFIG_X} ${CONFIG_WIDTH_GOAL}" > ${FILENAME}
	echo "${CONFIG_Y_PADDLE} ${CONFIG_X_PADDLE} ${CONFIG_WIDTH_PADDLE}" >> ${FILENAME}
	for BALL in ${CONFIG_SERVES}; do
		BALL_Y=$(echo $BALL | cut -d, -f1)
		BALL_X=$(echo $BALL | cut -d, -f2)
		BALL_SPEED_Y=$(echo $BALL | cut -d, -f3)
		BALL_SPEED_X=$(echo $BALL | cut -d, -f4)
		echo "${BALL_Y} ${BALL_X} ${BALL_SPEED_Y} ${BALL_SPEED_X}" >> ${FILENAME}
		echo "firaball=(${BALL_Y}),(${BALL_X}), (${BALL_SPEED_Y}),(${BALL_SPEED_X})"
	done
}


# num, min, max
util_is_in_range() {
	NUM=$1
	MIN=$2
	MAX=$3
	[ "${NUM}" -ge "${MIN}" ] && [ "${NUM}" -le "${MAX}" ]
}


# num, min, max
util_float_is_in_range() {
	NUM=$1
	MIN=$2
	MAX=$3
	(( $(echo "${NUM} >= ${MIN}" | bc -l) )) && (( $(echo "${MAX} <=  ${MAX}" | bc -l) ))
}

# min, max, promt
util_ask_for_input_in_range() {
	>&2 read -p "[$3] Introduce valor entre $1 y $2: " INPUT
	re='^[\-]?[0-9]+$'
	if ! [[ "${INPUT}" =~ ${re} ]] ; then
		err "bro, un numero prfa"
		util_ask_for_input_in_range $1 $2 "$3"
	fi
	if ! util_is_in_range ${INPUT} $1 $2; then
		err bro, entre $1 y $2
		util_ask_for_input_in_range $1 $2 "$3"
	fi
	return ${INPUT}
}

# num, min, max
util_float_is_in_range() {
	NUM=$1
	MIN=$2
	MAX=$3
	echo is $NUM between $MIN and $MAX
	(( $(echo "${NUM} >= ${MIN}" | bc -l) )) && (( $(echo "${NUM} <=  ${MAX}" | bc -l) ))
}


#OPT, MIN, MAX, prompt
validate() {
	VAR="$1"
	LOAD_VAR="LOAD_$1"
	CONFIG_VAR="CONFIG_$1"
	if [ "0" -eq ${!VAR} ]; then # no hay este VAR por param
		if [ "0" -eq ${!LOAD_VAR} ]; then # tampoco estaba en el fitxer (o no hay fitxer)
			util_ask_for_input_in_range $2 $3 "$4"
			declare -g ${CONFIG_VAR}="$?"
		else # darle valor que hay en el fitxer
			declare -g ${CONFIG_VAR}=${!LOAD_VAR}
		fi
	else # darle valor pasado por param
		declare -g ${CONFIG_VAR}=${!VAR}
	fi

	if ! util_is_in_range ${!CONFIG_VAR} $2 $3; then
		util_ask_for_input_in_range $2 $3 "$4"
		declare -g $CONFIG_VAR=$?
	fi
}


validate_config() {
	validate Y 10 120 "numFiles"
	validate X 10 36 "numColum"
	validate WIDTH_GOAL 8 $((CONFIG_Y-1)) "Mida porteria"

	validate Y_PADDLE 2 118 "posFilaPal"
	validate X_PADDLE 2 35 "posColPal"
	validate WIDTH_PADDLE 3 $((CONFIG_Y-1)) "midaPaleta"
	
	# get balls
	if [ "0" -eq "${N}" ]; then # si no pasa balls by param
		if [ "0" -eq "${LOAD_N}" ]; then # tampoco hay balls en el fitxer (o no hay fitxer)
			echo "bro no me has dado balls ni el config ni por params (con -1) :("
		else # no pasa balls por param pero si por fitxer
			N=${LOAD_N}
			SERVES=${LOAD_SERVES}
		fi
	else # si pasa balls por param, sobre-escribe las q haya en el fitxero
		CONFIG_N=${N}
		# CONFIG_SERVES=${SERVES}
		CONFIG_SERVES=""
	fi
	# validate balls
	N=0 # cuenta otra vez, just in case
	for BALL in ${SERVES}; do
		if [ "${N}" -lt "9" ]; then # en principio si llega aqui, N es correcta :/
			#TODO ATENCION! esto permite poner mis pelotas dentro Y FUERA de los rangos? numFiles?
			BALL_Y=$(echo ${BALL} | cut -d, -f1)
			if ! util_is_in_range ${BALL_Y} 2 118; then
				# echo Porque me estas pidiendo cositas? TODO
				util_ask_for_input_in_range 2 118 "posFilaPil_$N"
			fi
			BALL_X=$(echo ${BALL} | cut -d, -f2)
			if ! util_is_in_range ${BALL_X} 2 35; then
				util_ask_for_input_in_range 2 35 "posColPil_$N"
			fi
			BALL_SPEED_Y=$(echo ${BALL} | cut -d, -f3)
			while ! util_float_is_in_range ${BALL_SPEED_Y} -1 1; do
				>&2 read -p "[velFil_$N] Introduce valor decimal entre -1 y 1: " INPUT
				re='^[\-]?([0-9]+)|([0-9]*\.[0-9]+)$' # 9 or .9 or 0.9
				if [[ "${INPUT}" =~ ${re} ]] ; then
					BALL_SPEED_Y=$INPUT
				else
					err "bro, un float prfa"
				fi
			done
			BALL_SPEED_X=$(echo ${BALL} | cut -d, -f4)
			while ! util_float_is_in_range ${BALL_SPEED_X} -1 1; do
				>&2 read -p "[velCol_$N] Introduce valor decimal entre -1 y 1: " INPUT
				re='^[\-]?([0-9]+)|([0-9]*\.[0-9]+)$' # 9 or .9 or 0.9
				if [[ "${INPUT}" =~ ${re} ]] ; then
					BALL_SPEED_X=$INPUT
				else
					err "bro, un float prfa"
				fi
			done
			N=$((N+1))
			CONFIG_SERVES="${CONFIG_SERVES} ${BALL_Y},${BALL_X},${BALL_SPEED_Y},${BALL_SPEED_X}"
			echo "ADDINGBALLTO_CONFIG_SERVES: ${BALL_Y},${BALL_X},${BALL_SPEED_Y},${BALL_SPEED_X}"
		else
			err "demasiadas bolas bro"
			exit 0
		fi
	done
}


while getopts ':hn:f:c:p:0:m:1:' arg; do
	case "${arg}" in
		h)
			echo "	[-h] Mostra help"
			usage
			exit 0
			;;

		n)
			echo "	[-n nomFit] Nom del fitxer on guardar la config."
			if [ -z "${FILENAME}" ]; then
				FILENAME=${OPTARG}
				echo "FILENAME=${FILENAME}" # DEBUG
				# [ -f "$FILENAME" ] && load_config
			else
				err "me has dado fixer 2 veces!"
			fi
			;;

		f)
			echo "	[-f numFiles] # Files de la zona (10..120)"
			if [ "0" -eq "${Y}" ]; then
				Y=${OPTARG}
			else
				err "me has dado -f numFiles-1 2 veces!"
			fi
			;;

		c)
			echo "	[-c numColum] # Columnes de la zona (10..36)"
			if [ "0" -eq "${X}" ]; then
				X=${OPTARG}
			else
				err "me has dado -c numColum 2 veces!"
			fi
			;;

		p)
			echo "	[-p midaPort] Mida de la porteria (8..numFiles-1)"
			if [ "0" -eq "${WIDTH_GOAL}" ]; then
				WIDTH_GOAL=${OPTARG}
			else
				err "me has dado -p midaPort 2 veces!"
			fi
			;;

		0)
			echo "	[-0 posFilaPal,posColPal] Fila i columna de la paleta (2..118),(2..35)"
			if [ "0" -eq "${Y_PADDLE}" ] && [ "0" -eq "${X_PADDLE}" ]; then
				Y_PADDLE=$(echo "${OPTARG}" | cut -d',' -f1)
				X_PADDLE=$(echo "${OPTARG}" | cut -d',' -f2)
				echo="${Y_PADDLE} ${X_PADDLE}"
			else
				err "me has dado -0 (Fila y columna de la paleta) 2 veces!"
			fi
			;;

		m)
			echo "	[-m midaPaleta] Mida de la paleta (3..numFiles-1)"
			if [ "0" -eq "${WIDTH_PADDLE}" ]; then
				WIDTH_PADDLE=${OPTARG}
			else
				err "me has dado -m midaPaleta 2 veces!"
			fi
			;;

		1)
			# greater than
			for BALL in ${OPTARG}; do
				if [ "${N}" -lt "9" ]; then
					BALL_Y=$(echo $BALL | cut -d, -f1)
					BALL_X=$(echo $BALL | cut -d, -f2)
					BALL_SPEED_Y=$(echo $BALL | cut -d, -f3)
					BALL_SPEED_X=$(echo $BALL | cut -d, -f4)
					echo "reading ball #$N=(${BALL_Y}),(${BALL_X}), (${BALL_SPEED_Y}),(${BALL_SPEED_X})"
					if (( $(echo "${BALL_SPEED_Y} >= -1.0" | bc -l) )) &&
					 (( $(echo "${BALL_SPEED_Y} <=  1.0" | bc -l) )); then
						echo ok bola speed bien
					else
						echo no oka
					fi

					N=$((N+1))
					SERVES="${SERVES} ${BALL}"
				else
					err "demasiadas bolas bro"
					exit 0
				fi
			done
			# echo "y > 0"
			# echo "	[-1 posFilaPil,posColPil,velFil,velCol] Fila, columna, velocitat vertical i horitzontal de la pilota en joc (2..118),(2..35),(-1.0..1.0),(-1.0..1.0). Aquesta opt es pot repetir fins a 9 vegades."
			;;

		:)
			err "ERROR: Argument \"-${OPTARG}\" necesita parametres."
			>&2 usage 
			exit 1
			;;

		*)
			err "ERROR: Argument \"-${OPTARG}\" no reconegut."
			>&2 usage 
			exit 1
			;;

	esac
done


if [ -z "${FILENAME}" ]; then
	err No me has dado FILENAME con -n, adeu
	exit 1
fi
if [ -f "${FILENAME}" ]; then
	echo loading config cause it exists
	load_config
fi
echo validate_config
validate_config
echo save_config
save_config
echo pintas config======
cat $FILENAME

