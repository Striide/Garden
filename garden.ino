// This #include statement was automatically added by the Particle IDE.
#include "TimeAlarms/TimeAlarms.h"

// ************************* DEFINITIONS *************************
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

// Zone - Jumper mappings
#define ZONE1 3
#define ZONE2 4
#define ZONE3 5
#define ZONE4 6

// ************************* VARIABLES *************************

unsigned long last_cloud_time_sync = millis();
byte zone_state_array[4] = {0,0,0,0};
byte zones[4] = {ZONE1, ZONE2, ZONE3, ZONE4};
byte test_timer = 20;	// seconds for "relay test heartbeat timer"

void setup() {

	// set time zone to PST (daylight -6, std -7) on each boot
	set_time_zone(-7);

	// pin setup and initialization
	for (int i = 0; i < 4; i++){
		pinMode(zones[i], OUTPUT);
		digitalWrite(zones[i], LOW);
	}
	
	Spark.publish("striide.garden.online");
	Spark.function("runZone", runZone);
	Spark.function("allOff", zones_all_off);
}

void loop() {

}

// *************** Spark Functions **********************************
int runZone(String zone) {
    Spark.publish("striide.garden.runzone",zone);
    int zone_i = zone.toInt();
}

// ************************* ZONE FUNCTIONS *************************

void run_zone_for_duration(char zone_number, int duration){
	// turn on zone for duration

	// set timer and run
	change_zone_valve(zone_number, 1);
}

bool change_zone_valve(char zone_number, bool on_or_off){
	// turn the specified zone on or off
	String status;

	if (on_or_off){
		status = "on";
	} else {
		status = "off";
	}

	Spark.publish("striide.garden.runzone.valve", String(zone_number + ":" + status));

	return on_or_off;
}

void zones_all_off(){
	// turn all zones off

	Spark.publish("striide.garden.all_off");
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
// 	Spark.syncTime();
	last_cloud_time_sync = millis();

	Spark.publish("striide.garden.runzone");
}
