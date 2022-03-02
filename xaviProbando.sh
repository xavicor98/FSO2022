#! /bin/bash
		# ->Only info ; #// ->Problem or unsolved proposal

gamefield="-f $1 -c $2 -p $3"
racket="-0 $4,$5 -m $6"
firstball="-1 $7,$8,$9,$10"	#//Problem with vars from 10: Aren't recognizable
echo "$gamefield">config	#If doesn't exist creates a file; otherwise it overwrite it
echo "$racket">>config
echo "$firstball">>config	#//Try to put everything in a String connected by \n

i=0
while [ $i -le 8 ]
do
	if [  ]
	then
		
	fi
done
