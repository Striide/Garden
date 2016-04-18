// This #include statement was automatically added by the Particle IDE.
// #include "TimeAlarms/TimeAlarms.h"
#include "SparkTime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>  

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
String sString;
String dString;
unsigned long currentTime;
unsigned long stopTime;
    
UDP UDPClient;
SparkTime rtc;

void setup() {
    
    rtc.begin(&UDPClient, "north-america.pool.ntp.org");
    rtc.setTimeZone(-8); // gmt offset
    rtc.setUseDST(true);

	// pin setup and initialization
	for (int i = 0; i < NUM_ZONES; i++){
		pinMode(zones[i], OUTPUT);
		digitalWrite(zones[i], LOW);
	}
    sString = "Initialized and waiting...";
    dString = "No duration";
	
	Particle.publish("striide.garden.online");
	Particle.function("runZone", runZone);
	Particle.function("allOff", zones_all_off);
    Particle.variable("status", sString);
    Particle.variable("duration", dString);
}

void loop() {
    currentTime = rtc.nowEpoch();
    
    if(stopTime < currentTime)
    {
        zones_all_off("");
    }
}
int readZone(String command)
{
    int beginIdx = 0;
    int idx = command.indexOf(",");
    
    String zone_s = command.substring(beginIdx, idx);
	zone_s.trim();
    return zone_s.toInt();
}

int readDuration(String command)
{
    int beginIdx = 0;
    int idx = command.indexOf(",");
	
    beginIdx = idx + 1;
	String duration_s  = command.substring(beginIdx);
    duration_s.trim();
    return duration_s.toInt();
}


// *************** Particle Functions **********************************
int runZone(String command) {
    
    Serial.print("runZone...");
    Serial.println(command);
    
    int zone_i = readZone(command);
    int zone_duration = readDuration(command);
    Serial.println("zone: " + zone_i);
    Serial.println("duration: " + zone_duration);
    
    if(zone_i > 0 && zone_duration > 0)
    {
        
        // dString = "duration: " + zone_duration;
        Particle.publish("striide.garden.runzone",command);
        run_zone_for_duration(zone_i,zone_duration);
        return zone_i;
    }
    else
    {
        Particle.publish("striide.garden.error","Error processing runZone command");
        return 1;
    }
    
}

// ************************* ZONE FUNCTIONS *************************

void run_zone_for_duration(int zone_number, int duration){
	// turn on zone for duration
    // set the stopTime
    stopTime = currentTime + duration;
    change_zone_valve(zone_number,true);
    
    char str[80];
    sprintf(str, "duration(%d):stopTime(%lu):currentTime(%lu)", duration, stopTime, currentTime);
    
    Particle.publish("striide.garden.runzone.duration",str);
}

bool change_zone_valve(int zone_number, bool on_or_off){
	// turn the specified zone on or off
	String status;

    // turn other zones off
    zones_all_off("");
    
	if (on_or_off){
        status = "on";
        digitalWrite(zones[zone_number-1], HIGH);
        sString = zone_number + "is on";
	}

	//Particle.publish("striide.garden.runzone.valve", String(zone_number + ":" + status));
    
	return on_or_off;
}

int zones_all_off(String command){
	// turn all zones off

	Particle.publish("striide.garden.all_off");
    for (int i = 0; i < NUM_ZONES; i++){
		digitalWrite(zones[i], LOW);
	}
    sString = "All off";
    return 1;
}