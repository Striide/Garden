// This #include statement was automatically added by the Particle IDE.
// #include "TimeAlarms/TimeAlarms.h"

// ************************* DEFINITIONS *************************
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

// Zone - Jumper mappings
#define ZONE1 D3
#define ZONE2 D4
#define ZONE3 D5
#define ZONE4 D6
#define NUM_ZONES 4

// ************************* VARIABLES *************************

unsigned long last_cloud_time_sync = millis();
byte zone_state_array[NUM_ZONES] = {0,0,0,0};
byte zones[NUM_ZONES] = {ZONE1, ZONE2, ZONE3, ZONE4};
byte test_timer = 20;	// seconds for "relay test heartbeat timer"


void setup() {

	// pin setup and initialization
	for (int i = 0; i < NUM_ZONES; i++){
		pinMode(zones[i], OUTPUT);
		digitalWrite(zones[i], LOW);
	}
	
	Particle.publish("striide.garden.online");
	Particle.function("runZone", runZone);
	Particle.function("allOff", zones_all_off);
}

void loop() {

}

// *************** Particle Functions **********************************
int runZone(String zone) {
    Particle.publish("striide.garden.runzone",zone);
    int zone_i = zone.toInt();
    
    change_zone_valve(zone_i,true);
}

// ************************* ZONE FUNCTIONS *************************

void run_zone_for_duration(char zone_number, int duration){
	// turn on zone for duration
}

bool change_zone_valve(int zone_number, bool on_or_off){
	// turn the specified zone on or off
	String status;

	if (on_or_off){
        // turn other zones off
        zones_all_off("");
        
		status = "on";
        digitalWrite(zones[zone_number-1], HIGH);
	} else {
		status = "off";
        digitalWrite(zones[zone_number-1], LOW);
	}

	Particle.publish("striide.garden.runzone.valve", String(zone_number + ":" + status));
    
	return on_or_off;
}

int zones_all_off(String command){
	// turn all zones off

	Particle.publish("striide.garden.all_off");
    for (int i = 0; i < NUM_ZONES; i++){
		digitalWrite(zones[i], LOW);
	}
}