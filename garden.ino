// This #include statement was automatically added by the Particle IDE.
// #include "TimeAlarms/TimeAlarms.h"

// ************************* DEFINITIONS *************************
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

// Zone - Jumper mappings
#define ZONE1 D3
#define ZONE2 D4
#define ZONE3 D5
#define ZONE4 D6

// ************************* VARIABLES *************************

unsigned long last_cloud_time_sync = millis();
int num_of_zones = 4;
byte zone_state_array[num_of_zones] = {0,0,0,0};
byte zones[num_of_zones] = {ZONE1, ZONE2, ZONE3, ZONE4};
byte test_timer = 20;	// seconds for "relay test heartbeat timer"


void setup() {

	// set time zone to PST (daylight -6, std -7) on each boot
	set_time_zone(-7);

	// pin setup and initialization
	for (int i = 0; i < num_of_zones; i++){
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

	// set timer and run
	//change_zone_valve(zone_number, true);
}

bool change_zone_valve(int zone_number, bool on_or_off){
	// turn the specified zone on or off
	String status;

	if (on_or_off){
		status = "on";
        digitalWrite(zones[zone_number-1], HIGH);
	} else {
		status = "off";
        digitalWrite(zones[zone_number-1], LOW);
	}

	Particle.publish("striide.garden.runzone.valve", String(zone_number + ":" + status));
    
	return on_or_off;
}

int zones_all_off(){
	// turn all zones off

	Particle.publish("striide.garden.all_off");
    for (int i = 0; i < num_of_zones; i++){
		digitalWrite(zones[i], LOW);
	}
}

// ************************* TIME FUNCTIONS *************************

void set_time_zone(float offset){
	// set the current time zone
	Time.zone(offset);
}

bool check_sync_timer(){
	// check if 24 hours have passed
	if (millis() - last_cloud_time_sync > ONE_DAY_MILLIS) {
		return true;
	} else {
		return false;
	}
}

void sync_time_with_cloud(){
	// update current time on microprocessor
// 	Particle.syncTime();
	last_cloud_time_sync = millis();

	Particle.publish("striide.garden.runzone");
}
