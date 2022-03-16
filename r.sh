./test.sh -n lelele -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5" -1 "3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5"

echo "Pla de proves inicial:"
echo "   Prova 1: Passem tots els paràmetres i no hi ha fitxer (crea)."
./test.sh -n camp1.txt -f 25 -c 40 -p 20 –0 22,30 –m 10 -1 21,20,-1.0,0.3
#El resultat hauria de ser un fitxer amb el contingut: 25 40 20 / 22 30 10 / 21 20 -1.0.0.3

echo "   Prova 2: Passem tots els paràmetres però les files estan fora de rang (error)."
./test.sh -n camp1.txt -f 8 -c 40 -p 20 –0 22,30 –m 10 -1 21,20,-1.0,0.3
#Hauria de mostrar un error per pantalla i no es crea el fitxer

echo "   Prova 3: No passem la fila com a paràmetres i no hi ha fitxer (crea)."
./test.sh -n camp2.txt -c 40 -p 20 –0 22,30 –m 10 -1 21,20,-1.0,0.3
#Hauria de crear un fitxer amb el contingut: 25 40 20 / 22 30 10 / 21 20 -1.0.0.3

echo "   Prova 4: Modifiquem un fitxer."
./test.sh -n camp1.txt -f 50 -c 80 -p 20 
#Hauria de modificar el fitxer amb el contingut: 50 80 20 / 22 30 10 / 21 20 -1.0.0.3

echo "   Prova 5: Passem tots els paràmetres i no hi ha fitxer i posem dues piotes (crea)."
./test.sh -n camp1.txt -f 25 -c 40 -p 20 –0 22,30 –m 10 -1 21,20,-1.0,0.3 11,22,-1.0,0.3
#Hauria de crear el fitxer amb el contingut: 25 40 20 / 22 30 10 / 21 20 -1.0.0.3 / 11 22 -1.0.0.3

echo "Pla de proves ampliat:"
