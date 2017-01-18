from flask import Flask, render_template, request, redirect, url_for
from drone_tower_pyduino import *
import time

app = Flask(__name__)

# initialize connection to Arduino
# if your arduino was running on a serial port other than '/dev/ttyACM0/'
# declare: a = Arduino(serial_port='/dev/ttyXXXX')
a = Arduino()
time.sleep(3)

# declare the pins we're using
SERVO_PIN = 6

# initialize the digital pin as output
# a.set_pin_mode(LED_PIN,'O')

print('Arduino initialized')


# we are able to make 2 different requests on our webpage
# GET = we just type in the url
# POST = some sort of form submission like a button
@app.route('/', methods=['POST', 'GET'])
def initiate_auto_loader():

    # variables for template page (templates/index.html)
    author = "Harambe"

    # if we make a post request on the webpage aka press button then do stuff
    if request.method == 'POST':

        # if we press the Turn Green button
        if request.form['submit'] == 'Loading Mode':
            print('Loading mode')

            # turn on LED that is Green
            a.loading_mode()

        if request.form['submit'] == 'Unloading Mode':
            print('Unloading mode')

            # Turn LED's to work normally
            a.unloading_mode()

        else:
            pass

    # read in analog value from photoresistor
    time.sleep(1)
    # readval = a.analog_read(ANALOG_PIN)

    # the default page to display will be our template with our template variables
    '''value=100*(readval/1023.)'''
    return render_template('index.html', author=author, value=0)

if __name__ == "__main__":

    # lets launch our webpage!
    # do 0.0.0.0 so that we can log into this webpage
    # using another computer on the same network later
    app.run(host='0.0.0.0')
