from guizero import App, Text, PushButton, Picture
from datetime import datetime, timedelta

# Air controller variables
motor_speed = 0
motor_speed_text = ["Off", "Low", "High"]
pump_state = 0
pump_state_text = ["Off", "On"]
time_hold = False
hold_until_time = datetime.now()
time_holding_step = 15


def motor_button_clicked():
    global motor_speed
    global motor_speed_text
    global motor_text

    motor_speed += 1
    if (motor_speed >= len(motor_speed_text)):
        motor_speed = 0
    if motor_speed > 0:
        motor_text.text_color = 'green'
    else:
        motor_text.text_color = 'black'
    motor_text.value = motor_speed_text[motor_speed]
    print("speed: " + motor_speed_text[motor_speed])


def pump_button_clicked():
    global pump_state
    global pump_state_text
    global pump_text

    pump_state += 1
    if (pump_state >= len(pump_state_text)):
        pump_state = 0
    if pump_state > 0:
        pump_text.text_color = 'green'
    else:
        pump_text.text_color = 'black'
    pump_text.value = pump_state_text[pump_state]
    print("Pump: " + pump_state_text[pump_state])


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
        check_remaining_time()
        print("Hold until: " + hold_until_time.strftime("%H:%M"))


def refresh_time():
    global time_hold
    global time_text
    global hold_until_time
    global cancel_hold_button
    global time_subs_button

    if (time_hold):
        time_text.value = "Hold until:\n" + hold_until_time.strftime("%H:%M")
        time_text.text_color = 'green'
    else:
        time_text.value = "Current time:\n" + datetime.now().strftime("%H:%M")
        time_text.text_color = 'black'
        cancel_hold_button.disable()
        time_subs_button.disable()

# Run every second to check timer and holds


def check_remaining_time():
    global time_hold
    global hold_until_time
    global time_text
    global time_subs_button

    # No need to check anything if we don't have a hold
    if (time_hold):
        if (hold_until_time >= (datetime.now() + timedelta(minutes=time_holding_step))):
            time_subs_button.enable()
        else:
            time_subs_button.disable()

        remaining_time = hold_until_time - datetime.now()
        if (remaining_time.total_seconds() < 1):
            time_hold = False
            adjust_hold_time(0)
        print("Remaining time: " + str(remaining_time.total_seconds()))


# UI elements
element_height = 3
button_width = 10
time_button_width = 5
text_width = 12
font_size = 14
picture_size = 100

app = App(title="Cool Air Controller",
          width=480,
          height=320,
          layout="grid")
frost_picture = Picture(app,
                        image="frost.png",
                        grid=[3, 0],
                        height=picture_size,
                        width=picture_size)
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

# Set buttons text size
motor_button.text_size = font_size
pump_button.text_size = font_size
cancel_hold_button.text_size = font_size
time_subs_button.text_size = font_size
time_add_button.text_size = font_size

# UI loop
time_text.repeat(1000, check_remaining_time)
app.set_full_screen()
app.display()
