#! /bin/bash

KAKA=3
LOAD_KAKA=3

PEPE=3
LOAD_PEPE=3


func() {
	VAR="KAKA"
	OTRA_VAR="LOAD_KAKA"
	# $VAR=13 # esto no funca
	declare -g ${VAR}=$VALOR
	declare -g ${LOAD_VAR}=$VALOR
	echo he puesto VAR=${!VAR}
}

func

echo $KAKA
