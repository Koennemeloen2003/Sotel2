/*
 * Vars.h
 *
 * Created: 17-6-2025 12:58:41
 *  Author: kaben
 */ 


#ifndef VARS_H_
#define VARS_H_

typedef unsigned char byte;
byte display;

const uint16_t getallen[16]={
	0xBE,
	0xA0,
	0x3B,
	0xB9,
	0xA5,
	0x9D,
	0x9F,
	0xB0,
	0xBF,
	0xBD,
	0xB7,
	0x8F,
	0x0B,
	0xAB,
	0x1F,
	0x17
};

const uint16_t bitjes[8]={
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80
};

const uint16_t toneFreqs[11] = {
	0,      // '0' ? stil
	262,    // '1' ? C4
	294,    // '2' ? D4
	330,    // '3' ? E4
	349,    // '4' ? F4
	392,    // '5' ? G4
	440,    // '6' ? A4
	494,    // '7' ? B4
	523,    // '8' ? C5
	587,	// '9' ? D5
};



#endif /* VARS_H_ */