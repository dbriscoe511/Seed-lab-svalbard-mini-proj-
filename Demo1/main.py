import time
import comms
import sys
import computer_vision as cv
import subprocess

system = comms.comm() # initialize i2c and display

cmd = [sys.executable, "-c", "import computer_vision as cv; gains = cv.camera_setup(); cv.cv_main(gains)"]
process = subprocess.Popen(cmd, stdout=subprocess.PIPE)
#These values are hard coded into the arduino on demo1, no need for this function for the first demo. 
def excersize2(angle,dist):
    time.sleep(8)
    system.angle(angle)
    
    system.move(dist*12)

def excersize1():
    while (True):
        time.sleep(0.1)
        angle = process.stdout.readline()
        angle = angle.decode('utf-8')
        angle.strip('\n')
        print(angle)
        system.update_lcd(str(angle))

exr = int(input("what excersize? (1: camera angle test, 2 (obsolete): rotate/drive test)"))
if exr ==2:
    #angle(degree), dist
    excersize2(90,5)
else:
    excersize1()


