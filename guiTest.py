from guizero import App, Text, PushButton
from datetime import datetime, timedelta
import time
import argparse
try:
    import RPi.GPIO as GPIO
    relays_enabled = True
except ImportError:
    print("Probably Running in desktop")
    relays_enabled = False


def motor_button_clicked():
    global motor_speed

    motor_speed += 1
    if (motor_speed >= len(motor_speed_text)):
        motor_speed = 0
    set_motor_speed(motor_speed)


def pump_button_clicked():
    global pump_state

    pump_state += 1
    if (pump_state >= len(pump_state_text)):
        pump_state = 0
    set_pump(pump_state)


def time_subs_button_clicked():
    global time_hold
    global cancel_hold_button
    global time_holding_step

    adjust_hold_time(-time_holding_step)
    if (time_hold):
        cancel_hold_button.enable()


def time_add_button_clicked():
    global time_hold
    global cancel_hold_button
    global time_holding_step

    adjust_hold_time(time_holding_step)
    if (time_hold):
        cancel_hold_button.enable()


def cancel_hold_button_clicked():
    global cancel_hold_button
    global time_subs_button

    adjust_hold_time(0)
    cancel_hold_button.disable()
    time_subs_button.disable()


def exit_button_clicked():
    global app

    turn_off_all()
    app.destroy()
    print("Goodbye!")


def adjust_hold_time(delta_time=0):
    global time_hold
    global hold_until_time
    global time_subs_button

    if (delta_time == 0):
        print("Canceling hold")
        time_hold = False
    else:
        if (time_hold == False):
            # If we're starting the hold, use current time
            hold_until_time = datetime.now()

        hold_until_time += timedelta(minutes=delta_time)
        print("New hold until time: " + str(hold_until_time.second))
        time_hold = True

    refresh_time()
    if time_hold:
        print("Hold until: " + hold_until_time.strftime("%H:%M"))


def refresh_time():
    global time_hold
    global time_text
    global hold_until_time
    global cancel_hold_button
    global time_subs_button
    global text_color_active
    global text_color_inactive

    if (time_hold):
        time_text.value = "Hold until:\n" + hold_until_time.strftime("%H:%M")
        time_text.text_color = text_color_active
    else:
        time_text.value = "Current time:\n" + datetime.now().strftime("%H:%M")
        time_text.text_color = text_color_inactive
        cancel_hold_button.disable()
        time_subs_button.disable()


def check_remaining_time():
    # Run every second to check timer and holds
    global time_hold
    global hold_until_time
    global time_subs_button
    global time_holding_step

    # No need to check anything if we don't have a hold
    if (time_hold):
        if (hold_until_time >= (datetime.now() + timedelta(minutes=time_holding_step))):
            time_subs_button.enable()
        else:
            time_subs_button.disable()

        remaining_time = hold_until_time - datetime.now()
        if (remaining_time.total_seconds() < 0):
            # Time's up, we'll cancel the hold, and turn off everything
            print("Time's up")
            turn_off_all()
            time_hold = False
            adjust_hold_time(0)
        else:
            print("Remaining time: " + str(remaining_time.total_seconds()))


def init_pins():
    global relays_enabled
    global pin_motor_low
    global pin_motor_high
    global pin_pump

    if (relays_enabled):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(pin_motor_low, GPIO.OUT)
        GPIO.setup(pin_motor_high, GPIO.OUT)
        GPIO.setup(pin_pump, GPIO.OUT)
        GPIO.output(pin_motor_low, pin_off)
        GPIO.output(pin_motor_high, pin_off)
        GPIO.output(pin_pump, pin_off)


def set_motor_speed(speed):
    global relays_enabled
    global pin_motor_low
    global pin_motor_high
    global motor_speed_text
    global motor_text
    global text_color_active
    global text_color_inactive

    if (relays_enabled):
        # First shut down all motor relays, and wait a bit
        GPIO.output(pin_motor_low, pin_off)
        GPIO.output(pin_motor_high, pin_off)
        time.sleep(0.500)

        if (speed == 1):
            GPIO.output(pin_motor_low, pin_on)
        elif (speed == 2):
            GPIO.output(pin_motor_high, pin_on)

    if speed > 0:
        motor_text.text_color = text_color_active
    else:
        motor_text.text_color = text_color_inactive
    motor_text.value = motor_speed_text[speed]
    print("Motor: " + motor_speed_text[speed])


def set_pump(state):
    global relays_enabled
    global pin_pump
    global pump_state_text
    global pump_text
    global text_color_active
    global text_color_inactive

    if (relays_enabled):
        if (state == 0):
            GPIO.output(pin_pump, pin_off)
        elif (state == 1):
            GPIO.output(pin_pump, pin_on)

    if state > 0:
        pump_text.text_color = text_color_active
    else:
        pump_text.text_color = text_color_inactive
    pump_text.value = pump_state_text[state]
    print("Pump: " + pump_state_text[state])


def turn_off_all():
    global motor_speed
    global pump_state

    motor_speed = 0
    pump_state = 0
    set_motor_speed(motor_speed)
    set_pump(pump_state)
    print("System Off")


# Air controller variables
motor_speed = 0
motor_speed_text = ["Off", "Low", "High"]
pump_state = 0
pump_state_text = ["Off", "On"]
time_hold = False
hold_until_time = datetime.now()
time_holding_step = 30
if (relays_enabled):
    pin_motor_low = 5
    pin_motor_high = 6
    pin_pump = 13
    pin_off = GPIO.HIGH
    pin_on = GPIO.LOW

# UI elements
element_height = 3
button_width = 10
time_button_width = 4
text_width = 12
exit_button_size = 1
font_size = 16
picture_size = 100
bg_color = "#393F3F"
text_color_default = "white"
text_color_active = "#28C7CF"
text_color_inactive = "light gray"

app = App(title="Cool Air Controller",
          #   width=480,
          #   height=320,
          layout="grid",
          bg=bg_color)
motor_button = PushButton(app,
                          command=motor_button_clicked,
                          text="Motor speed",
                          grid=[0, 0],
                          width=button_width,
                          height=element_height)
motor_text = Text(app,
                  text=motor_speed_text[motor_speed],
                  grid=[1, 0],
                  size=font_size)
pump_button = PushButton(app,
                         command=pump_button_clicked,
                         text="Pump",
                         grid=[0, 1],
                         width=button_width,
                         height=element_height)
pump_text = Text(app,
                 text=pump_state_text[pump_state],
                 grid=[1, 1],
                 size=font_size)
cancel_hold_button = PushButton(app,
                                command=cancel_hold_button_clicked,
                                text="cancel",
                                enabled=False,
                                grid=[0, 2],
                                width=button_width,
                                height=element_height)
time_text = Text(app,
                 text=str("Current time:\n" +
                          datetime.now().strftime("%H:%M")),
                 grid=[1, 2],
                 width=text_width,
                 height=element_height,
                 size=font_size)
time_subs_button = PushButton(app,
                              command=time_subs_button_clicked,
                              text="-",
                              enabled=False,
                              grid=[2, 2],
                              width=time_button_width,
                              height=element_height)
time_add_button = PushButton(app,
                             command=time_add_button_clicked,
                             text="+",
                             grid=[3, 2],
                             width=time_button_width,
                             height=element_height)
exit_button = PushButton(app,
                         command=exit_button_clicked,
                         text="X",
                         grid=[3, 0],
                         width=exit_button_size,
                         height=exit_button_size,
                         align="top")

# Set buttons text size
motor_button.text_size = font_size
motor_button.text_color = text_color_default
pump_button.text_size = font_size
pump_button.text_color = text_color_default
cancel_hold_button.text_size = font_size
cancel_hold_button.text_color = text_color_default
time_subs_button.text_size = font_size
time_subs_button.text_color = text_color_default
time_add_button.text_size = font_size
time_add_button.text_color = text_color_default

motor_text.text_color = text_color_inactive
pump_text.text_color = text_color_inactive
time_text.text_color = text_color_inactive

parser = argparse.ArgumentParser()
parser.add_argument("--dry", action="store_true",
                    help="Dry run, relays don't turn on")
args = parser.parse_args()
if (args.dry):
    relays_enabled = False
    print("Dry run, relays won't turn on")

# UI loop
init_pins()
turn_off_all()
time_text.repeat(1000, check_remaining_time)
if (relays_enabled):
    app.set_full_screen()
app.display()
