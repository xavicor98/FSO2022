#!/usr/bin/env python3
import getopt, sys


def err(msg):
    print("ERROR: " + msg, file=sys.stderr)


class Config:
    filename=None # Nom del fitxer on guardar la config
    y = 0 # Files
    x = 0 # Cols
    width_goal = 0 # Mida porteria
    y_paddle = 0 # Fila pal
    x_paddle = 0 # Col pal
    width_paddle = 0 # Mida pal
    # n = 0 # Num. de pilotas
    serves = []
    # serves="" # Info posFilaPil posColPil velFil velCol de pilotas
    def validate():
        return True

def usage(err=False):
    if err:
        err("USAGE: " + sys.argv[0])
        err(" [-h] Mostra help")
        err(" [-n nomFit] Nom del fitxer on guardar la config.")
        err(" [-f numFiles] # Files de la zona (10..120)")
        err(" [-c numColum] # Columnes de la zona (10..36)")
        err(" [-p midaPort] Mida de la porteria (8..numFiles-1)")
        err(" [-0 posFilaPal,posColPal] Fila i columna de la paleta (2..118),(2..35)")
        err(" [-m midaPaleta] Mida de la paleta (3..numFiles-1)")
        err(" [-1 posFilaPil,posColPil,velFil,velCol] Fila, columna, velocitat vertical i horitzontal de la pilota en joc (2..118),(2..35),(-1.0..1.0),(-1.0..1.0). Aquesta opt es pot repetir fins a 9 vegades.")
    else:
        print("USAGE: " + sys.argv[0])
        print(" [-h] Mostra help")
        print(" [-n nomFit] Nom del fitxer on guardar la config.")
        print(" [-f numFiles] # Files de la zona (10..120)")
        print(" [-c numColum] # Columnes de la zona (10..36)")
        print(" [-p midaPort] Mida de la porteria (8..numFiles-1)")
        print(" [-0 posFilaPal,posColPal] Fila i columna de la paleta (2..118),(2..35)")
        print(" [-m midaPaleta] Mida de la paleta (3..numFiles-1)")
        print(" [-1 posFilaPil,posColPil,velFil,velCol] Fila, columna, velocitat vertical i horitzontal de la pilota en joc (2..118),(2..35),(-1.0..1.0),(-1.0..1.0). Aquesta opt es pot repetir fins a 9 vegades.")


def util_ask_for_input_in_range_float(min, max, prompt=""):
    while True:
        try:
            v = float(input(prompt + " dame input entre " + str(min) + " y " + str(max) + ": "))
            if min <= v <= max:
                return v
            print("bro, entre " + str(min) + " y " + str(max) + "...")
        except ValueError:
            print("bro, un float")
    return 0

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



def parse_args():
    cfg_args = Config()
    try:
        opts, args = getopt.getopt(sys.argv[1:], ":hn:f:c:p:0:m:1:")
    except getopt.GetoptError as e:
        # print help information and exit:
        print(e)  # will print something like "option -a not recognized"
        usage(err=True)
        sys.exit(2)
    for o, a in opts:
        if o == "-h":
            usage()
            # sys.exit()
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
                print("he leido una bola")
                if len(cfg_args.serves) < 9:
                    cfg_args.serves.append([ball_y, ball_x, ball_speed_y, ball_speed_x])
                else:
                    err("demasiadas bolas bro")
                    sys.exit(1)



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
                    cfg_load.y, cfg_load.x, cfg_load.width_goal = int(cfg_load.y), int(cfg_load.x), int(cfg_load.width_goal)
                elif n == 1:
                    cfg_load.y_paddle, cfg_load.x_paddle, cfg_load.width_paddle = line.split()
                    cfg_load.y_paddle, cfg_load.x_paddle, cfg_load.width_paddle = int(cfg_load.y_paddle), int(cfg_load.x_paddle), int(cfg_load.width_paddle)
                    pass
                else:
                    ball_y, ball_x, ball_speed_y, ball_speed_x = line.split()
                    cfg_load.serves.append([int(ball_y), int(ball_x), float(ball_speed_y), float(ball_speed_x)])

    except FileNotFoundError as e:
        # print("No hay file bro")
        print("fitxer \"" + filename + "\" no existe, lo creamos")
    cfg_load.filename = filename
    return cfg_load


def validate_config(cfg_args, cfg_load):
    cfg = Config()
    if cfg_args.y == 0:
        if cfg_load.y == 0:
            print("numFiles no esta ni en fitxer ni en los params")
        else:
            cfg.y = cfg_load.y
    else:
        cfg.y = cfg_args.y

    if cfg_args.x == 0:
        if cfg_load.x == 0:
            print("numColum no esta ni en fitxer ni en los params")
        else:
            cfg.x = cfg_load.x
    else:
        cfg.x = cfg_args.x

    if cfg_args.width_goal == 0:
        if cfg_load.width_goal == 0:
            print("midaPort no esta ni en fitxer ni en los params")
        else:
            cfg.width_goal = cfg_load.width_goal
    else:
        cfg.width_goal = cfg_args.width_goal

    if cfg_args.y_paddle == 0:
        if cfg_load.y_paddle == 0:
            print("posFilaPal no esta ni en fitxer ni en los params")
        else:
            cfg.y_paddle = cfg_load.y_paddle
    else:
        cfg.y_paddle = cfg_args.y_paddle

    if cfg_args.x_paddle == 0:
        if cfg_load.x_paddle == 0:
            print("posColPal no esta ni en fitxer ni en los params")
        else:
            cfg.x_paddle = cfg_load.x_paddle
    else:
        cfg.x_paddle = cfg_args.x_paddle


    if cfg_args.width_paddle == 0:
        if cfg_load.width_paddle == 0:
            print("midaPaleta no esta ni en fitxer ni en los params")
        else:
            cfg.width_paddle = cfg_load.width_paddle
    else:
        cfg.width_paddle = cfg_args.width_paddle

    # si no pasa balls by param
    if not cfg_args.serves:
        # si no pasa balls en fitxer
        if not cfg_load.serves:
            err("bro no me has dado balls ni el config ni por params")
            exit(1)
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

    tmp_serves = []
    for n, serve in enumerate(cfg.serves):
        ball_y, ball_x, ball_speed_y, ball_speed_x = serve
        print("eval serve#" + str(n) + " :: " + str(serve))
        #if ! util_is_in_range ${BALL_Y} 2 118; then
        if not 2 <= ball_y <= 118: ##TODO ATENCION! esto permite poner mis pelotas dentro Y FUERA de los rangos? numFiles?
            ball_y = util_ask_for_input_in_range(2, 118, "ball#" + str(n) + "_y")
        if not 2 <= ball_x <= 35: ##TODO ATENCION! esto permite poner mis pelotas dentro Y FUERA de los rangos? numFiles?
            ball_x = util_ask_for_input_in_range(2, 118, "ball#" + str(n) + "_x")
        if not -1.0 <= ball_speed_y <= 1.0:
            ball_speed_y = util_ask_for_input_in_range_float(-1.0, 1.0, "ball#" + str(n) + "speed_y")
        if not -1.0 <= ball_speed_x <= 1.0:
            ball_speed_x = util_ask_for_input_in_range_float(-1.0, 1.0, "ball#" + str(n) + "speed_x")
        tmp_serves.append([ball_y, ball_x, ball_speed_y, ball_speed_x])

    cfg.serves = tmp_serves
    cfg.filename = cfg_load.filename
    return cfg

def save_config(cfg):
    try:
        with open(cfg.filename, "w") as f:
            #f.write(str(cfg.y) + " " + str(cfg.x) + " " + str(cfg.width_goal) + "\n")
            f.write(f"{cfg.y} {cfg.x} {cfg.width_goal}\n")
            f.write(f"{cfg.y_paddle} {cfg.x_paddle} {cfg.width_paddle}\n")
            for serve in cfg.serves:
                ball_y, ball_x, ball_speed_y, ball_speed_x = serve
                f.write(f"{ball_x} {ball_x} {ball_speed_y} {ball_speed_x}\n")
    except OSError as e:
        err(str(e))
        err("No puedo guardar cfg en : \"" + cfg.filename + "\"")



def main():
    # cfg_args = Config()
    print("Hola desde main")
    cfg_args = parse_args()
    if cfg_args.filename is None:
        err("bro no hay file name o q")
        sys.exit(1)

    cfg_load = load_config(cfg_args.filename) # Info que ya hay en el fitxer

    cfg = validate_config(cfg_args, cfg_load)
    save_config(cfg)
    print("Adeu desde main")



if __name__ == "__main__":
    main()
