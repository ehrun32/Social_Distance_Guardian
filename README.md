# Social_Distance_Guardian(Covid-19 Project) 😷

*TV remote too far* (◐▂◑ ) 
<p> 💡Let me use REMOTEMATE to grab the remote! 😉 </p>
<p> WOOOO 😝 </p>
 

## Project Summary
The Social Distance Guardian is a beacon designed to monitor compliance with social distancing guidelines. The beacon emits a warning beep if it detects that two or more beacons are within a pre-set distance from each other, indicating that the individuals need to increase the distance between themselves to maintain social distancing.
This project was done by a team where I worked under my professor. My part of the project was programming the admin device and the beacon it self and the hardware implementation. 
For more information on this project please visit:
https://www.laviasolutions.ca/product

## Methodology
### This project has 2-parts
- Admin Device 
  - The device need a main device which we will call it admin device. This Device will control the the social distance guardian beacon and will allow certain functions to pass through.
- Social Distance Guardian 
  - The social distance guardian and the admin device use Bluetooth to communicate with each other. The Bluetooth module that is used in the device is called HM-10 module. Each device has two Bluetooth modules, one of them is the broadcaster. This broadcasts the signal to other devices. For the admin device, the broadcaster sends the command through the broadcaster to the beacons when a button is pressed respectively. For the beacons the broadcaster is always sending out a signal, if another two beacons are placed and one beacon comes in the range of the other beacons, the beacons will know because the broadcaster is always sending signals. The other Bluetooth module is the observer, this observes the data and send it to Arduino. For the admin the observer is only used to find the beacons ID, so it can send proper commands for each beacon. For the beacons as explained before, if another beacon comes in range, the observer will know that the beacon is in the range due to the other broadcaster sending its signal and the beacon will start alarming.

## Features
### User access levels
•	REGISTER: This will register the beacon for IN-CIRCLE to be activated.
•	IN-CIRCLE: If a group of people have two beacons the user has to activate IN-CIRCLE so the beacon doesn’t beep when it’s around.
•	SLEEP: This will put the beacon into sleep mode.
•	CALIBRATION: This will set the distance the user wants for the beacon.
•	REGISTER PHONE: This will register the phone for IN-CIRCLE to be activated.


## Prototype
The following section discusses the performance of the integrated prototype as a whole. The
prototype is completely functional and all promised features have been delivered. 

### Admin Device
The first prototype for the admin device are shown below. 

<p align="center">
  <img src="![image](https://user-images.githubusercontent.com/57046416/217137512-046d1e5d-38b9-4418-9ba3-fa388ed9a0e0.png)"/>
</p>

The first prototype for the admin device that will be implemented later on are shown below
<p align="center">
  <img src="![image](https://user-images.githubusercontent.com/57046416/217137621-4e044240-0499-42ce-afcb-55a0e046ebca.png)"/>
</p>


### Social Distance Guardian
The final prototype for the beacons are shown below.

<p align="center">
  <img src="![image](https://user-images.githubusercontent.com/57046416/217138114-6e0c8e42-46d5-46d4-b032-f3723ee40d22.png)"/>
</p>

<p align="center">
  <img src="![image](https://user-images.githubusercontent.com/57046416/217138219-2b58eec1-a8e4-4786-a3a6-eee325be80e6.png)"/>
</p>



## Next steps

Things to improve:
- Make a cell phone application that does the same thing as social distance guardian




## References

- Social Distancing Guardian. (n.d.). Retrieved September 14, 2020, from https://www.laviasolutions.ca/

