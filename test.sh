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




while getopts ':hn:f:c:p:0:m:1:' arg; do
	case "${arg}" in
		h) 
			echo "	[-h] Mostra help"
			usage
			exit 0
			;;
		n) 
			echo "	[-n nomFit] Nom del fitxer on guardar la config."
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
			echo "	[-1 posFilaPil,posColPil,velFil,velCol] Fila, columna, velocitat vertical i horitzontal de la pilota en joc (2..118),(2..35),(-1.0..1.0),(-1.0..1.0). Aquesta opt es pot repetir fins a 9 vegades."
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
