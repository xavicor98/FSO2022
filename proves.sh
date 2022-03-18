echo "Pla de proves inicial:"
echo "   Prova 1: Passem tots els paràmetres i no hi ha fitxer (crea)."
./test.sh -n camp1.txt -f 25 -c 36 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
#El resultat hauria de ser un fitxer amb el contingut: 25 36 20 / 22 30 10 / 21 20 -1.0 0.3

echo "   Prova 2: Passem tots els paràmetres però les files estan fora de rang (error)."
./test.sh -n camp1.txt -f 8 -c 36 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
#Hauria de mostrar un error per pantalla i no es crea el fitxer

echo "   Prova 3: No passem la fila com a paràmetres i no hi ha fitxer (crea)."
./test.sh -n camp2.txt -c 36 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
#Hauria de crear un fitxer amb el contingut: ?? 36 20 / 22 30 10 / 21 20 -1.0 0.3

echo "   Prova 4: Modifiquem un fitxer."
./test.sh -n camp1.txt -f 50 -c 30 -p 20 
#Hauria de modificar el fitxer amb el contingut: 50 30 20 / 22 30 10 / 21 20 -1.0 0.3

echo "   Prova 5: Passem tots els paràmetres i no hi ha fitxer i posem dues piotes (crea)."
./test.sh -n camp1.txt -f 25 -c 36 -p 20 -0 22,30 -m 10 -1 "21,20,-1.0,0.3 11,22,-1.0,0.3"
#Hauria de crear el fitxer amb el contingut: 25 36 20 / 22 30 10 / 21 20 -1.0 0.3 / 11 22 -1.0 0.3

echo "Pla de proves ampliat:"


#./test.sh -n lelele -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5" -1 "3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5"

# Test con 10 bolas (demasiadas)
rm -f err1.txt
./test.sh -n err1.txt -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5" -1 "3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5 10,10,-1.0,-1.0"
rm -f err1.txt
./config.py -n err1. -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5" -1 "3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5 10,10,-1.0,-1.0"
rm -f err1.txt
./config.py -n err1. -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5 3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5 10,10,-1.0,-1.0"
rm -f err1.txt
./config.py -n err1.txt -1 "1,1,-1.0,-1.0" -1 "2,2,-1.0,0.5" -1 "3,3,-1.0,0.5" -1 "4,4,-1.0,0.5" -1 "5,5,-1.0,0.5" -1 "6,6,-1.0,0.5" -1 "7,7,-1.0,0.5" -1 "8,8,-1.0,0.5" -1 "9,9,-1.0,0.5" -1 "10,10,-1.0,-1.0"
