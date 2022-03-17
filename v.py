def util_ask_for_input_in_range(min, max, prompt=""):
    while True:
        try:
            v = int(input(prompt + " dame input entre " + str(min) + " y " + str(max) + ": "))
            if not min <= v <= max:
                return v
            print("bro, entre " + str(min) + " y " + str(max) + "...")
        except ValueError:
            print("bro, un int")
    return 0

util_ask_for_input_in_range(10, 120, "si")
