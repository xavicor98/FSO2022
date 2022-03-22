echo "Pla de proves inicial:"
echo "   Prova 1: Passem tots els paràmetres i no hi ha fitxer (crea)."
./config.sh -n camp1.txt -f 25 -c 36 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
#El resultat hauria de ser un fitxer amb el contingut: 25 36 20 / 22 30 10 / 21 20 -1.0 0.3

echo "   Prova 2: Passem tots els paràmetres però les files estan fora de rang (error)."
./config.sh -n camp1.txt -f 8 -c 36 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
#Hauria de mostrar un error per pantalla i no es crea el fitxer

echo "   Prova 3: No passem la fila com a paràmetres i no hi ha fitxer (crea)."
./config.sh -n camp2.txt -c 36 -p 20 -0 22,30 -m 10 -1 21,20,-1.0,0.3
#Hauria de crear un fitxer amb el contingut: ?? 36 20 / 22 30 10 / 21 20 -1.0 0.3

echo "   Prova 4: Modifiquem un fitxer."
./config.sh -n camp1.txt -f 50 -c 30 -p 20 
#Hauria de modificar el fitxer amb el contingut: 50 30 20 / 22 30 10 / 21 20 -1.0 0.3

echo "   Prova 5: Passem tots els paràmetres i no hi ha fitxer i posem dues piotes (crea)."
./config.sh -n camp1.txt -f 25 -c 36 -p 20 -0 22,30 -m 10 -1 "21,20,-1.0,0.3 11,22,-1.0,0.3"
#Hauria de crear el fitxer amb el contingut: 25 36 20 / 22 30 10 / 21 20 -1.0 0.3 / 11 22 -1.0 0.3

#---------------------------------------------------------------------------

echo "Pla de proves ampliat:"

echo "   Prova 1: Es creen mes pilotes de le spermeses (error)"
# Test amb 10 boles (supera el màx)
rm -f err1.txt
./config.sh -n err1.txt -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5" -1 "3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5 10,10,-1.0,-1.0"
rm -f err1.txt
./config.sh -n err1. -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5" -1 "3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5 10,10,-1.0,-1.0"
rm -f err1.txt
./config.py -n err1. -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5 3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5 10,10,-1.0,-1.0"
rm -f err1.txt
./config.py -n err1.txt -1 "1,1,-1.0,-1.0" -1 "2,2,-1.0,0.5" -1 "3,3,-1.0,0.5" -1 "4,4,-1.0,0.5" -1 "5,5,-1.0,0.5" -1 "6,6,-1.0,0.5" -1 "7,7,-1.0,0.5" -1 "8,8,-1.0,0.5" -1 "9,9,-1.0,0.5" -1 "10,10,-1.0,-1.0"

echo "   Prova 2: Dimensio de porteria mes gran de la permesa (demana mida nova)"
./config.sh -n camp1.txt -f 25 -c 36 -p 40 -0 22,30 -m 10 -1 21,20,-1.0,0.3
./config.py -n camp1.txt -f 25 -c 36 -p 85 -0 22,30 -m 10 -1 21,20,-1.0,0.3

echo "   Prova 3: Pilotes creades fora de rang (demana posicio nova"
./config.sh -n camp1.txt -f 25 -c 36 -p 20 -0 27,30 -m 10 -1 26,20,-1.0,0.3
./config.py -n camp1.txt -f 25 -c 36 -p 20 -0 20,35 -m 10 -1 22,50,-1.0,0.3

echo "   Prova 4: "
./config.sh -n camp1.txt -f 25 -c 36 -p 20 -0 21,30 -m 10 -1 21,20,-1.0,0.3
./config.py -n camp1.txt -f 25 -c 36 -p 23 -0 24,30 -m 10 -1 21,20,-1.0,0.3

echo "   Prova 5: "
./config.sh -n camp1.txt -f 25 -c 36 -p 20 -0 27,30 -m 10 -1 21,20,-1.0,0.3
./config.py -n camp1.txt -f 25 -c 36 -p 23 -0 34,30 -m 10 -1 21,20,-1.0,0.3


#./test.sh -n lelele -1 "1,1,-1.0,-1.0 2,2,-1.0,0.5" -1 "3,3,-1.0,0.5 4,4,-1.0,0.5 5,5,-1.0,0.5 6,6,-1.0,0.5 7,7,-1.0,0.5 8,8,-1.0,0.5 9,9,-1.0,0.5"
