import time
import comms
import computer_vision 

previous_angle = 1
while True:
    #check comp vision, get angle
    angle = 2
    measured_angle = 0
    time.sleep(0.5)
    if not angle == previous_angle:
        comms.send(angle)
        time.sleep(0.5)
        measured_angle = comms.read()
        comms.update_lcd_ecnoder(measured_angle*90)
        previous_angle = angle

    