# porn-arduino
P.O.R.N. Automatic Plant Watering System based on Arduino

Automatically keeps plants watered using a small pump, soil humidity sensors, solenoid valves and relays


16 Relay board 'MH' (there is no brand printed)
	
	The relay is on the 'NC' (normally closed) state when there is no electricity applied to it.
	The COM (common) pin is connected to ground (DC-) on the transformer and interconnected with Jumper cables in the board.
	The DC+ is connected with Jumper cables to all the pluses of connected devices.
	Closing the ground on the control pins 1 to 16 (LOW) will activate the relay and make it close the NO (normally open) pin,
		activating the device connected to it.
	Therefore: 
		All the ground of the devices are connected to the relays, all the plus sides are connected directly to the + on the transformer. 
	Pins
		Arduino		Board	Device
		2 			1		Pump
		3			2		Solenoid
		...
		13			12		Solenoid
		23			13		Solenoid
	 
	 
Conventions
  - Stripped wires and Black wires when connected alone are COM (DC-, negative, ground)
  - Black wires when connected in a wire strip are just signal
  
Solenoids
	There is a Diode connected on each solenoid to avoid current from flowing from DC- (ground) to DC+. 
	The diode is the component with the gray stripe. The side with the stripe (cathode, negative) should be connected to the PLUS side (positive, power)
	
Pump 
	Also has a diode on the power side.
	

Diodes
	Diodes should be connected with their negative side on the positive side of the load