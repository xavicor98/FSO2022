#!/usr/bin/env python3
import getopt, sys

def usage():
    print("USAGE: " + sys.argv[0])
    print(" [-h] Mostra help")
    print(" [-n nomFit] Nom del fitxer on guardar la config.")
    print(" [-f numFiles] # Files de la zona (10..120)")
    print(" [-c numColum] # Columnes de la zona (10..36)")
    print(" [-p midaPort] Mida de la porteria (8..numFiles-1)")
    print(" [-0 posFilaPal,posColPal] Fila i columna de la paleta (2..118),(2..35)")
    print(" [-m midaPaleta] Mida de la paleta (3..numFiles-1)")
    print(" [-1 posFilaPil,posColPil,velFil,velCol] Fila, columna, velocitat vertical i horitzontal de la pilota en joc (2..118),(2..35),(-1.0..1.0),(-1.0..1.0). Aquesta opt es pot repetir fins a 9 vegades.")


def util_ask_for_input_in_range(min, max, prompt=""):
    while True:
        try:
            v = int(input(prompt + " dame input entre " + str(min) + " y " + str(max) + ": "))
            if min <= v <= max:
                return v
            print("bro, entre " + str(min) + " y " + str(max) + "...")
        except ValueError:
            print("bro, un int")
    return 0


class Config:
    filename=None # Nom del fitxer on guardar la config
    y=0 # Files
    x=0 # Cols
    width_goal=0 # Mida porteria
    y_paddle=0 # Fila pal
    x_paddle=0 # Col pal
    width_paddle=0 # Mida pal
    # n=0 # Num. de pilotas
    serves = []
    # serves="" # Info posFilaPil posColPil velFil velCol de pilotas
    def validate():
        return True

def parse_args():
    cfg_args = Config()
    try:
        opts, args = getopt.getopt(sys.argv[1:], ":hn:f:c:p:0:m:1:", ["help", "output="])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(err)  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    output = None
    verbose = False
    for o, a in opts:
        if o == "-h":
            usage()
            sys.exit()
        elif o == "-n": # nomFit
            if cfg_args.filename is None:
                cfg_args.filename = a
                print("he leido fitxer de args")
            else:
                print("bro, me has pasado el fitxer 2 veces")
        elif o == "-f":
            if cfg_args.y == 0:
                try:
                    cfg_args.y = int(a)
                except ValueError:
                    print("bro, numFiles tiene q ser int, no \"" + a +"\"")
                    sys.exit(1)
                print("he leido numFiles de args")
            else:
                print("bro, me has pasado numFiles 2 veces")

        elif o == "-c":
            if cfg_args.x == 0:
                try:
                    cfg_args.x = int(a)
                except ValueError:
                    print("bro, numColum tiene q ser int, no \"" + a +"\"")
                    sys.exit(1)
                print("he leido numColum de args")
            else:
                print("bro, me has pasado numColum 2 veces")

        elif o == "-p":
            if cfg_args.width_goal == 0:
                try:
                    cfg_args.width_goal = int(a)
                except ValueError:
                    print("bro, midaPort tiene q ser int, no \"" + a +"\"")
                    sys.exit(1)
                print("he leido midaPort de args")
            else:
                print("bro, me has pasado midaPort 2 veces")



        elif o == "-0":
            if cfg_args.y_paddle == 0 or cfg_args.x_paddle == 0 :
                try:
                    #a = "posFilaPal,posColPal"
                    cfg_args.y_paddle, cfg_args.x_paddle = a.split(',')
                    cfg_args.y_paddle = int(cfg_args.y_paddle)
                    cfg_args.x_paddle = int(cfg_args.x_paddle)
                except ValueError:
                    print("bro, midaPort tiene q ser int,int  (no \"" + a +"\")")
                    sys.exit(1)
                print("he leido midaPort de args: " + str(cfg_args.y_paddle) + "," + str(cfg_args.x_paddle))
            else:
                print("bro, me has pasado midaPort 2 veces")

        elif o == "-m":
            if cfg_args.width_paddle == 0:
                try:
                    cfg_args.width_paddle = int(a)
                except ValueError:
                    print("bro, midaPaleta tiene q ser int, no \"" + a +"\"")
                    sys.exit(1)
                print("he leido midaPaleta de args")
            else:
                print("bro, me has pasado midaPaleta 2 veces")

        elif o == "-1":
            for serve_str in a.split(" "):
                print("leo serve #" + str(len(cfg_args.serves)) + "    "+ serve_str)
                # TODO comprova q de verdad hay 4 valores
                ball_y, ball_x, ball_speed_y, ball_speed_x = serve_str.split(',')
                try:
                    ball_y = int(ball_y)
                    ball_x = int(ball_x)
                    ball_speed_y = float(ball_speed_y)
                    ball_speed_x = float(ball_speed_x)
                except ValueError:
                    print("error leer bola#" + str(len(cfg_args.serves)) + " bro, cuando pasas bola, q sea tipo [posFilaPil,posColPil,velFil,velCol]  , no (\"" + a + "\")")
                    sys.exit(1)
                print("he leido uma bola")
                cfg_args.serves.append([ball_y, ball_x, ball_speed_y, ball_speed_x])



        elif o in ("-o", "--output"):
            output = a
        else:
            assert False, "unhandled option"

    return cfg_args


def load_config(filename):
    cfg_load = Config()
    try:
        with open(filename) as f:
            for n,line in enumerate(f):
                if n == 0:
                    cfg_load.y, cfg_load.x, cfg_load.width_goal = line.split()
                elif n == 1:
                    cfg_load.y_paddle, cfg_load.x_paddle, cfg_load.width_paddle = line.split()
                    pass
                else:
                    ball_y, ball_x, ball_speed_y, ball_speed_x = line.split()
                    cfg_load.serves.append([int(ball_y), int(ball_x), float(ball_speed_y), float(ball_speed_x)])

    except FileNotFoundError as e:
        print("No hay file bro")

    return cfg_load


def validate_config(cfg_args, cfg_load):
    cfg = Config()
    if cfg_args.y == 0:
        if cfg_load.y == 0:
            print("numFiles no esta ni en fitxer ni en los params")
        else:
            cfg.y = cfg_load
    else:
        cfg.y = cfg_args

    if cfg_args.x == 0:
        if cfg_load.x == 0:
            print("numColum no esta ni en fitxer ni en los params")
        else:
            cfg.x = cfg_load
    else:
        cfg.x = cfg_args

    if cfg_args.width_goal == 0:
        if cfg_load.width_goal == 0:
            print("midaPort no esta ni en fitxer ni en los params")
        else:
            cfg.width_goal = cfg_load
    else:
        cfg.width_goal = cfg_args

    if cfg_args.y_paddle == 0:
        if cfg_load.y_paddle == 0:
            print("posFilaPal no esta ni en fitxer ni en los params")
        else:
            cfg.y_paddle = cfg_load
    else:
        cfg.y_paddle = cfg_args

    if cfg_args.x_paddle == 0:
        if cfg_load.x_paddle == 0:
            print("posColPal no esta ni en fitxer ni en los params")
        else:
            cfg.x_paddle = cfg_load
    else:
        cfg.x_paddle = cfg_args


    if cfg_args.width_paddle == 0:
        if cfg_load.width_paddle == 0:
            print("midaPaleta no esta ni en fitxer ni en los params")
        else:
            cfg.width_paddle = cfg_load
    else:
        cfg.width_paddle = cfg_args

    if not cfg_args.serves: # si no pasa balls by param
        if not cfg_load.serves: # si no pasa balls en fitxer
            print("bro no me has dado balls ni el config ni por params (con -1) :(")
        else:
            cfg.serves = cfg_load.serves
    else:
        cfg.serves = cfg_args.serves

    # validate Y 10 120 "numFiles"
    if not 10 <= cfg.y <= 120:
        cfg.y = util_ask_for_input_in_range(10, 120, "numFiles")

    # validate X 10 36 "numColum"
    if not 10 <= cfg.x <= 36:
        cfg.x = util_ask_for_input_in_range(10, 36, "numColum")

    # validate WIDTH_GOAL 8 $((CONFIG_Y-1)) "Mida porteria"
    if not 8 <= cfg.width_goal <= cfg.y - 1:
        cfg.width_goal = util_ask_for_input_in_range(8, cfg.y - 1, "midaPort")

    # validate Y_PADDLE 2 118 "posFilaPal"
    if not 2 <= cfg.y_paddle <= 118:
        cfg.y_paddle = util_ask_for_input_in_range(8, 118, "posFilaPal")

    # validate X_PADDLE 2 35 "posColPal"
    if not 2 <= cfg.x_paddle <= 35:
        cfg.x_paddle = util_ask_for_input_in_range(2, 35, "posColPal")

    # validate WIDTH_PADDLE 3 $((CONFIG_Y-1)) "midaPaleta"
    if not 3 <= cfg.width_paddle <= cfg.y - 1:
        cfg.x_paddle = util_ask_for_input_in_range(3, cfg.y - 1, "midaPaleta")

    # TODO validate BOLAS
    # ### #  (bash):
	# ### # # validate balls
	# ### # N=0 # cuenta otra vez, just in case
	# ### # for BALL in ${SERVES}; do
	# ### # 	if [ "${N}" -lt "9" ]; then # en principio si llega aqui, N es correcta :/
	# ### # 		#TODO ATENCION! esto permite poner mis pelotas dentro Y FUERA de los rangos? numFiles?
	# ### # 		BALL_Y=$(echo ${BALL} | cut -d, -f1)
    return cfg


def main():
    # cfg_args = Config()
    print("Hola desde main")
    cfg_args = parse_args()
    if cfg_args.filename is None:
        print("bro no hay file name o q")
        sys.exit(1)

    cfg_load = load_config(cfg_args.filename) # Info que ya hay en el fitxer

    cfg = validate_config(cfg_args, cfg_load)
    print("Adeu desde main")



if __name__ == "__main__":
    main()
