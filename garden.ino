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
String sString;  // general status string
String dString;  // duration string
String zString;  // zone string
unsigned long currentTime;
unsigned long stopTime = 0;
    
UDP UDPClient;
SparkTime rtc;

void setup() {

		rtc.begin(&UDPClient, "north-america.pool.ntp.org");
		rtc.setTimeZone(-8); // gmt offset
		rtc.setUseDST(true);
		
		// pin setup and initialization
		for (int i = 0; i < NUM_ZONES; i++)
		{
				pinMode(zones[i], OUTPUT);
				digitalWrite(zones[i], LOW);
		}
		sString = "Initialized and waiting...";
		dString = "-";
		zString = "-";
		
		Particle.publish("striide.garden.online");
		
		Particle.function("runZone", runZone);
		Particle.function("allOff", zones_all_off);
		Particle.variable("status", sString);
		Particle.variable("duration", dString);
		Particle.variable("zone", zString);
}


void loop() {
		currentTime = rtc.nowEpoch();
		
		if(stopTime > 0 && stopTime < currentTime)
		{
				stopTime = 0;
				zones_all_off("");
		}
		
		// wait 5 seconds
		delay(5000);
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
		
		int zone_i = readZone(command);
		int zone_duration = readDuration(command);
		
		if(zone_i > 0 && zone_duration > 0)
		{
				Particle.publish("striide.garden.runzone");
				
				dString = "" + zone_duration;
				zString = "" + zone_i;
				
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

// turn on zone for duration
void run_zone_for_duration(int zone_number, int duration)
{
		stopTime = currentTime + duration;
		change_zone_valve(zone_number,true);
}

// turn the specified zone on or off
bool change_zone_valve(int zone_number, bool on_or_off)
{	
		// turn other zones off
		zones_all_off_helper();
		
		if (on_or_off)
		{
				digitalWrite(zones[zone_number-1], HIGH);
				sString = "on";
		}
		else
		{
				sString = "off";
		}
		
		return on_or_off;
}
int zones_all_off_helper()
{
		for (int i = 0; i < NUM_ZONES; i++){
				digitalWrite(zones[i], LOW);
		}
		sString = "off";
		return 1;
}

// turn all zones off
int zones_all_off(String command)
{
		Particle.publish("striide.garden.all_off");
		stopTime = 0;
		return zones_all_off_helper();
}