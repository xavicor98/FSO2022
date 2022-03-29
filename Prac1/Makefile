.PHONY: all run clean test entrega

all:
	mkdir -p tmp
	./config.py -h
	./config.sh -h


run:
	mkdir -p tmp
	./config.py -n tmp/tmp1.txt -1 "2,2,-1.0,-1.0"
	./config.sh -h tmp/tmp2.txt -1 "2,2,-1.0,-1.0"


clean:
	rm -f tmp/*

test:
	echo hola bb
	# ./proves/provataLTABLALBA.sh


entrega:
	rm -f entrega.tgz
	tar -czf entrega.tgz config.py proves.sh config.sh proves/*
