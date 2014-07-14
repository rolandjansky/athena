/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CaloEvent/CPMTobRoI.h"

#include "CpmRoiSubBlockV2.h"

namespace LVL1BS {

// Constant definitions

const int CpmRoiSubBlockV2::s_wordIdVal;

const int CpmRoiSubBlockV2::s_glinkPins;
const int CpmRoiSubBlockV2::s_energyLen;
const int CpmRoiSubBlockV2::s_isolLen;
const int CpmRoiSubBlockV2::s_locationLen;
const int CpmRoiSubBlockV2::s_bunchCrossingBits;


CpmRoiSubBlockV2::CpmRoiSubBlockV2()
{
}

CpmRoiSubBlockV2::~CpmRoiSubBlockV2()
{
}

// Clear all data

void CpmRoiSubBlockV2::clear()
{
  L1CaloSubBlock::clear();
  m_roiData.clear();
}

// Store header

void CpmRoiSubBlockV2::setRoiHeader(const int version, const int crate,
                                                       const int module)
{
  setHeader(s_wordIdVal, version, NEUTRAL, 0, crate, module, 0, 1);
}

// Store RoI

void CpmRoiSubBlockV2::fillRoi(const LVL1::CPMTobRoI roi)
{
  const LVL1::CPMTobRoI roiTemp(crate(), module(), 0, 0, 0, 0, 0);
  if (roi.crate() == roiTemp.crate() && roi.cpm() == roiTemp.cpm()) {
    m_roiData.resize(2*s_glinkPins);
    const int pin = (roi.chip() << 1) |
                    ((roi.location() >> s_locationLen) & 0x1);
    const int type = roi.type(); // em or tau (0/1)
    m_roiData[2*pin+type] = roi;
  }
}

// Return RoI for given chip and location (left/right) and type (em/tau)

LVL1::CPMTobRoI CpmRoiSubBlockV2::roi(const int chip, const int loc,
                                                      const int type) const
{
  const int pin = (chip << 1) | (loc & 0x1);
  if (pin < s_glinkPins && !m_roiData.empty()) return m_roiData[2*pin+type];
  else return LVL1::CPMTobRoI(0);
}

// Packing/Unpacking routines

bool CpmRoiSubBlockV2::pack()
{
  bool rc = false;
  switch (version()) {
    case 2:                                     // <<== CHECK
      switch (format()) {
        case NEUTRAL:
	  rc = packNeutral();
	  break;
        default:
	  break;
      }
      break;
    default:
      break;
  }
  return rc;
}

bool CpmRoiSubBlockV2::unpack()
{
  bool rc = false;
  switch (version()) {
    case 2:                                     // <<== CHECK
      switch (format()) {
        case NEUTRAL:
	  rc = unpackNeutral();
	  break;
        default:
	  setUnpackErrorCode(UNPACK_FORMAT);
	  break;
      }
      break;
    default:
      setUnpackErrorCode(UNPACK_VERSION);
      break;
  }
  return rc;
}

// Pack neutral data

bool CpmRoiSubBlockV2::packNeutral()
{
  m_roiData.resize(2*s_glinkPins);
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    // RoI data
    const int idx = 2*pin;
    const LVL1::CPMTobRoI& roiEm(m_roiData[idx]);
    const LVL1::CPMTobRoI& roiTau(m_roiData[idx+1]);
    packerNeutral(pin, roiEm.energy(), s_energyLen);
    packerNeutral(pin, roiEm.isolation(), s_isolLen);
    packerNeutral(pin, 0, 1); //parity
    packerNeutral(pin, roiTau.energy(), s_energyLen);
    packerNeutral(pin, roiTau.isolation(), s_isolLen);
    packerNeutral(pin, 0, 1); //parity
    packerNeutral(pin, 0, 1); //error
    packerNeutral(pin, (roiEm.location()|roiTau.location()), s_locationLen);
    // Bunch Crossing number
    if (pin < s_bunchCrossingBits) {
      packerNeutral(pin, bunchCrossing() >> pin, 1);
    } else packerNeutral(pin, 0, 1);
    // G-Link parity
    packerNeutralParity(pin);
  }
  return true;
}

// Unpack neutral data

bool CpmRoiSubBlockV2::unpackNeutral()
{
  m_roiData.resize(2*s_glinkPins);
  int bunchCrossing = 0;
  for (int pin = 0; pin < s_glinkPins; ++pin) {
    // RoI data
    const int energyEm = unpackerNeutral(pin, s_energyLen);
    const int isolEm   = unpackerNeutral(pin, s_isolLen);
    unpackerNeutral(pin, 1); //parity
    const int energyTau = unpackerNeutral(pin, s_energyLen);
    const int isolTau   = unpackerNeutral(pin, s_isolLen);
    unpackerNeutral(pin, 1); //parity
    unpackerNeutral(pin, 1); //error
    const int loc = unpackerNeutral(pin, s_locationLen) |
                                           ((pin & 0x1) << s_locationLen);
    const int chip = pin >> 1;
    const int idx = 2*pin;
    m_roiData[idx] = LVL1::CPMTobRoI(crate(), module(), chip, loc, 0,
                                                        energyEm, isolEm);
    m_roiData[idx+1] = LVL1::CPMTobRoI(crate(), module(), chip, loc, 1,
                                                      energyTau, isolTau);
    // Bunch Crossing number
    if (pin < s_bunchCrossingBits) {
      bunchCrossing |= unpackerNeutral(pin, 1) << pin;
    } else unpackerNeutral(pin, 1);
    // G-Link parity error
    unpackerNeutralParityError(pin);
  }
  setBunchCrossing(bunchCrossing);
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

} // end namespace
