# Social_Distance_Guardian(Covid-19 Project) 😷

*BEEP BEEP BEEP* 😦
<p> WHOA BUDDY! YOU ARE TO CLOSE TO ME, BACK UP 😷 </p>
<p> NO MORE COVID 😄 </p>
 

## Project Summary
The Social Distance Guardian is a beacon designed to monitor compliance with social distancing guidelines. The beacon emits a warning beep if it detects that two or more beacons are within a pre-set distance from each other, indicating that the individuals need to increase the distance between themselves to maintain social distancing.
This project was done by a team where I worked under my professor. My part of the project was programming the admin device and the beacon it self and the hardware implementation. 
For more information on this project please visit:
https://www.laviasolutions.ca/product

## Methodology
### This project has 2-parts
- Admin Device 
  - The device need a main device which we will call it admin device. This Device will control the social distance guardian beacon and will allow certain functions to pass through.
- Social Distance Guardian 
  - The Social Distance Guardian and the Admin Device communicate with each other using Bluetooth technology. The device utilizes an HM-10 Bluetooth module for this purpose. Each device has two Bluetooth modules: a broadcaster and an observer. The broadcaster sends signals to other devices and the observer receives and sends data to the Arduino. In the case of the Admin Device, the broadcaster sends commands to the beacons when a button is pressed, while the observer is used to identify the beacons' IDs. On the other hand, the beacons constantly broadcast signals and the observer detects when another beacon comes in range, triggering an alarm.
  
## Features
### User access levels
- REGISTER: This will register the beacon for IN-CIRCLE to be activated.
- IN-CIRCLE: If a group of people have two beacons the user has to activate IN-CIRCLE so the beacon doesn’t beep when it’s around.
- SLEEP: This will put the beacon into sleep mode.
- CALIBRATION: This will set the distance the user wants for the beacon.
- REGISTER PHONE: This will register the phone for IN-CIRCLE to be activated.

### User Friendly
- Family members  alarm deactivation
- Longer distance threshold for higher risk users
- Master device in a store can control other devices
- Group battery charging Stackable design for each battery charging 
- Easy disinfection for circulation


## Prototype
The following section discusses the performance of the integrated prototype as a whole. The
prototype is completely functional and all promised features have been delivered. 

### Admin Device
The first prototype for the admin device are shown below. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/57046416/217137512-046d1e5d-38b9-4418-9ba3-fa388ed9a0e0.png"/>
</p>

The first prototype for the admin device that will be implemented later on are shown below
<p align="center">
  <img src="https://user-images.githubusercontent.com/57046416/217137621-4e044240-0499-42ce-afcb-55a0e046ebca.png"/>
</p>


### Social Distance Guardian
The final prototype for the beacons are shown below.

<p align="center">
  <img src="https://user-images.githubusercontent.com/57046416/217138114-6e0c8e42-46d5-46d4-b032-f3723ee40d22.png"/>
</p>

<p align="center">
  <img src="https://user-images.githubusercontent.com/57046416/217138824-bc000baa-7981-49da-82f0-e7d5d14fad06.png"/>
</p>



## Next steps

Things to improve:
- Make the range better and make a better admin cover 




## References

- Social Distancing Guardian. (n.d.). Retrieved September 14, 2020, from https://www.laviasolutions.ca/

