#! /bin/bash


err() {
	echo "[$(date +'%Y-%m-%dT%H:%M:%S%z')]: $*" >&2
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
SERVES=""



load_config() {
	echo "loading config from \"$FILENAME\" :D"
	while read -r line; do
		echo $line
	done < ${FILENAME}
}

save_config() {
	echo "${Y} ${X} ${WIDTH_GOAL}" > ${FILENAME}
	echo "${Y_PADDLE} ${X_PADDLE} ${WIDTH_PADDLE}" >> ${FILENAME}
	for BALL in ${SERVES}; do
		BALL_Y=$(echo $BALL | cut -d, -f1)
		BALL_X=$(echo $BALL | cut -d, -f2)
		BALL_SPEED_Y=$(echo $BALL | cut -d, -f3)
		BALL_SPEED_X=$(echo $BALL | cut -d, -f4)
		echo "firaball=(${BALL_Y}),(${BALL_X}), (${BALL_SPEED_Y}),(${BALL_SPEED_X})"
	done
}



# test empty
if [ -z "${Y}" ]; then
	echo "hey"
fi

# greater than
if [ "${Y}" -gt "0" ]; then
	echo "y > 0"
fi


# exit 0


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
				load_config
			else
				err "me has dado fixer 2 veces!"
			fi

			;;
		f) 
			echo "	[-f numFiles] # Files de la zona (10..120)"
			;;
		c) 
			echo "	[-c numColum] # Columnes de la zona (10..36)"
			;;
		p) 
			echo "	[-p midaPort] Mida de la porteria (8..numFiles-1)"
			;;
		0) 
			echo "	[-0 posFilaPal,posColPal] Fila i columna de la paleta (2..118),(2..35)"
			;;
		m)
			echo "	[-m midaPaleta] Mida de la paleta (3..numFiles-1)"
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
						echo ok1
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
			err "ERROR: Argument \"-${OPTARG}\" necesita parametres"
			>&2 usage 
			exit 1
			;;
		*)	
			err "ERROR: Argument \"-${OPTARG}\" misteri"
			>&2 usage 
			exit 1
			;;
	esac
done

save_config


