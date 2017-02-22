/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTgcDigitInfoUH
#define sTgcDigitInfoUH

#include <iosfwd>
#include <inttypes.h>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

class sTgcDigitInfo : public MuonDigit {

 public:  // functions

  enum {BC_UNDEFINED=0, BC_PREVIOUS, BC_CURRENT, BC_NEXT};
	sTgcDigitInfo() :  m_bcTag (BC_UNDEFINED), m_charge(-1), m_time(0), m_isDead (0), m_isPileup(0) { }

	//**********************************************************************
	sTgcDigitInfo(const Identifier& id, uint16_t bctag, float time, float charge, bool isDead, bool isPileup)
	  : MuonDigit(id),
			m_bcTag(bctag),
			m_charge(charge),
			m_time(time),
	    m_isDead(isDead),
	    m_isPileup(isPileup) { }
	// //**********************************************************************

	uint16_t bcTag() const {
	  return m_bcTag;
	}

	// get the charge
	float charge() const {
	  return m_charge;
	}

	// get the charge
	float time() const {
	  return m_time;
	}

	//return the dead channel status
	bool isDead() const {
		return m_isDead;
	}

	// return whether the digit is due to pileup
	bool isPileup() const {
		return m_isPileup;
	}

	void set_bcTag(uint16_t newbcTag) {
	  m_bcTag = newbcTag;
	}

	void set_charge(float newCharge) {
	  m_charge = newCharge;
	}

	void set_isDead(bool newIsDead) {
		m_isDead = newIsDead;
	}

	void set_isPileup(bool newIsPileup) {
		m_isPileup = newIsPileup;
	}

 private:  // data
  uint16_t  m_bcTag;
  float m_charge;
  float m_time;
  bool m_isDead;
  bool m_isPileup;
};

#endif
