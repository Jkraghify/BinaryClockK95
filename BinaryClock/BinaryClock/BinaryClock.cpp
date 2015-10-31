//BinaryClock.exe
//Created by Jkraghify on an uneventful night, October 2015
//Displays a binary clock on the G keys of the Corsair K95 RGB.
//Pls don't make fun of my poor programming, I'm teaching myself :P

#include "CUESDK.h"		// For Lighting functions
#include <ctime>		// For Current time
#include <iostream>		// For console messages
#include <thread>		// For program "sleeping"
#include <bitset>		// For easily converting decimal to binary - please don't hat me for being too lazy to write my own converter
#include <iomanip>		// For "setfill" and "setw" in console
#include <windows.h>	// For keypress listening ("escape")
#include <fstream>		// For reading Settings

float temp;														// Temporary variable
bool SettingsCorrupt = false;									// 
float Led[2][9] = {												//
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },								// Using a bidimentional array lets me take advantage of binary data
	{ 255, 255, 255, 255, 255, 255, 255, 255, 255 }				// "Off" corresponds to row "0" and "On corresponds to row "1"
};																// This eliminates the need for "if 0 then" and "if 1 then" statements

int LightKey(std::bitset<6> h, std::bitset<6> m, std::bitset<6> s) {
	int bits[3][6] {
		{ h[0], h[1], h[2], h[3], h[4], h[5] },					//
		{ m[0], m[1], m[2], m[3], m[4], m[5] },					// Master array for all bitsets
		{ s[0], s[1], s[2], s[3], s[4], s[5] }					//
	};
	CorsairLedId  id[3][6] ={
		{ CLK_G16, CLK_G13, CLK_G10, CLK_G7, CLK_G4, CLK_G1 },	// hours
		{ CLK_G17, CLK_G14, CLK_G11, CLK_G8, CLK_G5, CLK_G2 },	// minutes
		{ CLK_G18, CLK_G15, CLK_G12, CLK_G9, CLK_G6, CLK_G3 }	// seconds
};
	
	for (int i = 0; i < 6; i++) {								// This loop reads each bitset. The bitsets are 6 bits long (for six lights)
		int k = 0;												// to navigate colums of the Led bidimentional array
		for (int j = 0; j < 3; j++) {
			auto ledColor = CorsairLedColor{ id[j][i], Led[bits[j][i]][k], Led[bits[j][i]][k + 1], Led[bits[j][i]][k + 2] };
			CorsairSetLedsColors(1, &ledColor);
			k = k + 3;
		}
	}
	return 1;
}

int Conv2Binary(int Hour, int Min, int Sec) {
	std::bitset<6> h(Hour);		//
	std::bitset<6> m(Min);		//Assigns Hours (h), Minutes (m), and Seconds (s) to respective sets of six bits
	std::bitset<6> s(Sec);		//
	LightKey(h, m, s);			//runs Lightkey function with perameters h, m, and s defined in the previous lines
	std::cout << std::setfill('0') << std::setw(2) << Hour << ":" << std::setfill('0') << std::setw(2) << Min << ":" << std::setfill('0') << std::setw(2) << Sec << "||" << h << ":" << m << ":" << s << "     " << '\r';	//console display as a representation of what you should be seeing on the keyboard
	return 2;
}

int BinaryClock(int UpdateDelay) {
	time_t currentTime;						//
	struct tm localTime;					//
	time(&currentTime);						//
	localtime_s(&localTime, &currentTime);	// Gets current time, and localizes it
	Conv2Binary(localTime.tm_hour, localTime.tm_min, localTime.tm_sec);	//passes time information to be converted to binary
	std::this_thread::sleep_for(std::chrono::seconds(UpdateDelay));		// Sleeps the program so it doesn't use much CPU
	return 3;
}

const char* toString(CorsairError error)	//Default Corsair Error messages (they are nice, so I won't bother changing them)
{
	switch (error) {
	case CE_Success : 
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

int settings() {
	std::ifstream bcSettings("BCSettings.txt");						// Opens settings file located in working directory
	if (!bcSettings.is_open()) {
		std::cout << "Error opening settings file." << std::endl;	// Displays error if settings file does not exist
		SettingsCorrupt = true;										//
	}																//
	while (bcSettings.good()) {										//
		bcSettings >> temp;											// Stores settings in temporary variable
		if (temp < 0 || temp > 255) {								// Checks if temporary variable correlates to valid LED brightnesses
			std::cout << "Value in settings file is out of bounds. Using default values." << std::endl;
			SettingsCorrupt = true;
		}
	}
	if (!SettingsCorrupt) {											//
		bcSettings.clear();											// After check passes, return to beginning of file and store settings in the Led bidimentional array
		bcSettings.seekg(0, std::ios::beg);							//
		bcSettings >> Led[1][0] >> Led[1][1] >> Led[1][2] >> Led[0][0] >> Led[0][1] >> Led[0][2] >> Led[1][3] >> Led[1][4] >> Led[1][5] >> Led[0][3] >> Led[0][4] >> Led[0][5] >> Led[1][6] >> Led[1][7] >> Led[1][8] >> Led[0][6] >> Led[0][7] >> Led[0][8];
	}
	bcSettings.close();												// Closes settings file
	return 4;
}

int main(int argc, char * argv[])
{
	std::cout << "________________________________________________" << std::endl;	//
	std::cout << "This program is compatible with only the K95 RGB" << std::endl;	// Intro
	std::cout << "************************************************" << std::endl;	//
	std::cout << "Created by \"Jkraghify\" on the Corsair User Forums." << '\n' << std::endl;
	CorsairPerformProtocolHandshake();												//
	if (const auto error = CorsairGetLastError()) {									//
		std::cout << "Handshake failed: " << toString(error) << std::endl;			// Part of Corsair's Error messages. I refused to touch these.
		getchar();																	//
		return -1;																	//
	}																				//
	settings();																		// Grabs settings from the settings file
	std::cout << "Binary Clock is running. Click the \"X\" button to close." << std::endl;
	std::cout << "*******************************************************" << std::endl;
	while (1) {																		// Runs the BinaryClock function forever
		BinaryClock(1);																//
	}
    return 0;
}

/*
   ___ _                    _     _  __       
  |_  | |                  | |   (_)/ _|      
    | | | ___ __ __ _  __ _| |__  _| |_ _   _ 
    | | |/ / '__/ _` |/ _` | '_ \| |  _| | | |
/\__/ /   <| | | (_| | (_| | | | | | | | |_| |
\____/|_|\_\_|  \__,_|\__, |_| |_|_|_|  \__, |
                       __/ |             __/ |
                      |___/             |___/ 
*/