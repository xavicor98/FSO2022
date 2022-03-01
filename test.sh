#! /bin/bash

usage() {
	echo "USAGE:"
	echo "${0} [-h] [-f argsF] [-x argsX]"
}




while getopts 'h:f:x:' arg; do
	case "${arg}" in
		h) 
			echo "opcio h" 
			usage
			exit 0
			aflag='true'
			dea="${OPTARG}"
			echo "$OPTIND"
			;;
		f) 
			echo "opcio f"
			bflag='true'
			deb="${OPTARG}"
			echo "$OPTIND"
			;;
		x) 
			echo "opcio x"
			file="${OPTARG}"
			echo "$OPTIND"
			;;
		*) 
			echo "Unexpected option:\"${arg}\""
			usage 1>&2
			exit 1
			;;
	esac
done
