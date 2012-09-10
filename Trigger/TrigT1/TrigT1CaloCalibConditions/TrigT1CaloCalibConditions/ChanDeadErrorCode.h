/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_CHANDEADERRORCODE_H
#define TRIGT1CALOCALIBCONDITIONS_CHANDEADERRORCODE_H

#include "TrigT1CaloCalibConditions/AbstractErrorCode.h"

#include <string>

/**
 *  ChanDeadErrorCode class
 *  for L1Calo error codes
 *  Adapted from /LVL1/l1calo/coolL1Calo/coolL1Calo/ChanDeadCode.h
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class ChanDeadErrorCode: public AbstractErrorCode {

	friend std::ostream& operator<<(std::ostream& output, const ChanDeadErrorCode& r);

public:

	enum ChanDeadBits {
		// Bits used for CPM/JEM dead channel folders.
		BadLvdsBit = 0,

		// Bits used for PPM dead (or problem) channel folder.
		// Bits 0-15 will zero the LUT if they are set.
		// Bits 16-31 are flagging problems that do not need
		// the LUT to be zeroed.

		// Internal L1Calo problems:
		BadAdcBit = 0,
		BadMcmBit = 1,

		// Calorimeter problems:
		FrontEndDeadBit = 4,
		ReceiverDeadBit = 5,
		HighVoltsOffBit = 6,
		LowVoltsOffBit = 7,
		NoisyTowerBit = 8,

		// Calorimeter flags:
		// (for Tile, can use A=Front, B/C=Middle, D=Back)
		PSCellsMaskedBit = 16,
		FrontCellsMaskedBit = 17,
		MiddleCellsMaskedBit = 18,
		BackCellsMaskedBit = 19
	};

	ChanDeadErrorCode();
	ChanDeadErrorCode(unsigned int errorCode);
	virtual ~ChanDeadErrorCode() {}

	ChanDeadErrorCode(const ChanDeadErrorCode& e);
	ChanDeadErrorCode& operator=(const ChanDeadErrorCode& e);
	ChanDeadErrorCode& operator|=(const ChanDeadErrorCode& e);

	bool chanValid() const {return (( this->errorCode() & 0xffff) == 0);}

	bool badLvds() const { return bitIsSet(BadLvdsBit); }
	void badLvds(bool bad) { setBit(BadLvdsBit,bad); }

	bool badAdc() const { return bitIsSet(BadAdcBit); }
	void badAdc(bool bad) { setBit(BadAdcBit,bad); }

	bool badMcm() const { return bitIsSet(BadMcmBit); }
	void badMcm(bool bad) { setBit(BadMcmBit,bad); }

	bool frontEndDead() const { return bitIsSet(FrontEndDeadBit); }
	void frontEndDead(bool bad) { setBit(FrontEndDeadBit,bad); }

	bool receiverDead() const { return bitIsSet(ReceiverDeadBit); }
	void receiverDead(bool bad) { setBit(ReceiverDeadBit,bad); }

	bool highVoltsOff() const { return bitIsSet(HighVoltsOffBit); }
	void highVoltsOff(bool bad) { setBit(HighVoltsOffBit,bad); }

	bool lowVoltsOff() const { return bitIsSet(LowVoltsOffBit); }
	void lowVoltsOff(bool bad) { setBit(LowVoltsOffBit,bad); }

	bool noisyTower() const { return bitIsSet(NoisyTowerBit); }
	void noisyTower(bool bad) { setBit(NoisyTowerBit,bad); }

	bool psCellsMasked() const { return bitIsSet(PSCellsMaskedBit); }
	void psCellsMasked(bool bad) { setBit(PSCellsMaskedBit,bad); }

	bool frontCellsMasked() const { return bitIsSet(FrontCellsMaskedBit); }
	void frontCellsMasked(bool bad) { setBit(FrontCellsMaskedBit,bad); }

	bool middleCellsMasked() const { return bitIsSet(MiddleCellsMaskedBit); }
	void middleCellsMasked(bool bad) { setBit(MiddleCellsMaskedBit,bad); }

	bool backCellsMasked() const { return bitIsSet(BackCellsMaskedBit); }
	void backCellsMasked(bool bad) { setBit(BackCellsMaskedBit,bad); }

	std::string printStatus() const;
};

#endif


