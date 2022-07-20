###############################################################################
# Evaporative Air controller
###############################################################################

import argparse
from pathlib import Path
from datetime import datetime
import time
from copy import copy
import sys
import json
try:
    import RPi.GPIO as GPIO
    relays_enabled = True
except ImportError:
    print("Probably Running in desktop")
    relays_enabled = False

###############################################################################
# Constants
###############################################################################
SCRIPT_VERSION: int = 1
PROJECT_NAME: str = "Evaporative Air controller"
SEPARATOR: str = "###############################################################################"
# Files
SCRIPT_FOLDER = Path(__file__).parent.resolve()
CONFIG_FILE: str = str(SCRIPT_FOLDER)+"/airConf.json"
MOTOR_SPEED_MAX: int = 2
PUMP_STATE_MAX: int = 1
TIME_HOLD_MAX: int = 1
# Configuration
DEF_CONF_JSON: json = {
    "motor": 0,
    "pump": 0,
    "time_hold": False,
    "hold_until_time": str(datetime.now())
}
INV_MOTOR: int = -1
INV_PUMP: int = -1
INV_TIME_HOLD: int = -1
# GPIOs
if (relays_enabled):
    PIN_LEVEL_OFF = GPIO.HIGH
    PIN_LEVEL_ON = GPIO.LOW
    PIN_MOTOR_LOW = 5
    PIN_MOTOR_HIGH = 6
    PIN_PUMP = 13

###############################################################################
# Variables
###############################################################################
verbose_mode: bool = False

conf_json: json = copy(DEF_CONF_JSON)


###############################################################################
# Functions
###############################################################################


def exit_script(process_ok: bool):
    """Exit script depending on result"""
    status: int = 0
    exit_message: str = PROJECT_NAME
    print(SEPARATOR)
    if (process_ok):
        print(exit_message, " Done!")
    else:
        print(exit_message, " Failed!")
        status = 1
    sys.exit(status)


def set_motor(value: int) -> bool:
    """Validate the motor speed"""
    global conf_json
    if ((value >= 0) and (value <= MOTOR_SPEED_MAX)):
        conf_json["motor"] = value

        if (relays_enabled):
            global PIN_MOTOR_LOW
            global PIN_MOTOR_HIGH
            # First shut down all motor relays, and wait a bit
            GPIO.output(PIN_MOTOR_LOW, PIN_LEVEL_OFF)
            GPIO.output(PIN_MOTOR_HIGH, PIN_LEVEL_OFF)
            time.sleep(0.500)
            if (value == 1):
                GPIO.output(PIN_MOTOR_LOW, PIN_LEVEL_ON)
            elif (value == 2):
                GPIO.output(PIN_MOTOR_HIGH, PIN_LEVEL_ON)

        return True
    else:
        return False


def set_pump(value: int) -> bool:
    """Validate the pump state"""
    global conf_json
    if ((value >= 0) and (value <= PUMP_STATE_MAX)):
        conf_json["pump"] = value

    if (relays_enabled):
        global PIN_PUMP

        if (value == 0):
            GPIO.output(PIN_PUMP, PIN_LEVEL_OFF)
        elif (value == 1):
            GPIO.output(PIN_PUMP, PIN_LEVEL_ON)

        return True
    else:
        return False


def set_time_hold(value: str) -> bool:
    """Validate the pump state"""
    global conf_json
    result: bool = True
    if (value.casefold() == "false".casefold()):
        conf_json["time_hold"] = "false"
    elif (value.casefold() == "true".casefold()):
        conf_json["time_hold"] = "true"
    else:
        result = False
    return result


def load_conf_file() -> bool:
    """Load the configuration file data"""
    global conf_json

    if(verbose_mode):
        print("Opening file", CONFIG_FILE)
    try:
        conf_file = open(CONFIG_FILE, "r")
        conf_json = json.loads(conf_file.read())
        conf_file.close()
    except:
        print("Failed opening file")
        return False

    return True


def save_conf_file() -> bool:
    """Save the configuration JSON file"""
    global conf_json
    if(verbose_mode):
        print("Writing to file", CONFIG_FILE)
    try:
        conf_file = open(CONFIG_FILE, "w")
        conf_file.write(json.dumps(conf_json, indent=4))
        conf_file.close()
    except:
        print("Failed creating file")
        return False
    print("Saved configuration file")
    return True


###############################################################################
# Input arguments
###############################################################################
parser = argparse.ArgumentParser(
    description=PROJECT_NAME+" Release script version "+str(SCRIPT_VERSION))
parser.add_argument(
    "-v", "--verbose", help="Print helpful messages", action="store_true")
parser.add_argument("-m", "--motor", help="Motor speed",
                    dest="motor_speed", default=INV_MOTOR)
parser.add_argument("-p", "--pump", help="Pump state",
                    dest="pump_state", default=INV_PUMP)
parser.add_argument("-l", "--hold", help="Time hold",
                    dest="time_hold", default=INV_TIME_HOLD)


###############################################################################
# Script start
###############################################################################
if __name__ == "__main__":
    print(SEPARATOR)
    print(PROJECT_NAME + " Version " + str(SCRIPT_VERSION))

    args = parser.parse_args()

    if (args.verbose):
        verbose_mode = True
        print("Printing helpful messages")

    if not (load_conf_file()):
        print("Creating default configuration file")
        save_conf_file()

    if(args.motor_speed != INV_MOTOR):
        try:
            motor_speed: int = int(args.motor_speed)
        except:
            motor_speed = INV_MOTOR
        if(set_motor(motor_speed)):
            print("New motor speed:", str(conf_json["motor"]))
        else:
            print("Wrong motor speed", args.motor_speed)
            exit_script(False)

    if(args.pump_state != INV_PUMP):
        try:
            pump_state: int = int(args.pump_state)
        except:
            pump_state = INV_PUMP
        if(set_pump(pump_state)):
            print("New pump state:", str(conf_json["pump"]))
        else:
            print("Wrong pump state", args.pump_state)
            exit_script(False)

    if(args.time_hold != INV_TIME_HOLD):
        if(set_time_hold(args.time_hold)):
            print("New hold:", str(conf_json["time_hold"]))
        else:
            print("Wrong hold", args.time_hold)
            exit_script(False)

    if(verbose_mode):
        print("Configuration:")
        print("motor_speed:", conf_json["motor"])
        print("pump_state:", conf_json["pump"])
        print("time_hold:", conf_json["hold"])
        print("hold_until_time:", conf_json["hold_until_time"])

    save_conf_file()
    exit_script(True)
