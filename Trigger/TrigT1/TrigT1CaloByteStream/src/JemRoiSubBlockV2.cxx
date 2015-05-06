/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CaloEvent/JEMTobRoI.h"

#include "JemRoiSubBlockV2.h"

namespace LVL1BS {

// Constant definitions

const int JemRoiSubBlockV2::s_wordIdVal;

const int JemRoiSubBlockV2::s_frames;
const int JemRoiSubBlockV2::s_framesPerPin;
const int JemRoiSubBlockV2::s_bunchCrossingPin;
const int JemRoiSubBlockV2::s_energySmallBits;
const int JemRoiSubBlockV2::s_energyLargeBits;
const int JemRoiSubBlockV2::s_locationBits;
const int JemRoiSubBlockV2::s_bunchCrossingBits;
const int JemRoiSubBlockV2::s_paddingBits;


JemRoiSubBlockV2::JemRoiSubBlockV2()
{
}

JemRoiSubBlockV2::~JemRoiSubBlockV2()
{
}

// Clear all data

void JemRoiSubBlockV2::clear()
{
  L1CaloSubBlock::clear();
  m_roiData.clear();
}

// Store header

void JemRoiSubBlockV2::setRoiHeader(const int version, const int crate,
                                    const int module)
{
  setHeader(s_wordIdVal, version, NEUTRAL, 0, crate, module, 0, 1);
}

// Store RoI

void JemRoiSubBlockV2::fillRoi(const LVL1::JEMTobRoI roi)
{
  const LVL1::JEMTobRoI roiTemp(crate(), module(), 0, 0, 0, 0);
  if (roi.crate() == roiTemp.crate() && roi.jem() == roiTemp.jem()) {
    m_roiData.resize(s_frames);
    const int pos = roi.frame();
    m_roiData[pos] = roi;
  }
}

// Return RoI for given frame

LVL1::JEMTobRoI JemRoiSubBlockV2::roi(const int frame) const
{
  if (frame >= 0 && frame < s_frames && !m_roiData.empty()) {
    return m_roiData[frame];
  } else return LVL1::JEMTobRoI(0);
}

// Packing/Unpacking routines

bool JemRoiSubBlockV2::pack()
{
  bool rc = false;
  switch (version()) {
  case 1:                                               //<< CHECK
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

bool JemRoiSubBlockV2::unpack()
{
  bool rc = false;
  switch (version()) {
  case 2:                                               //<< CHECK
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

bool JemRoiSubBlockV2::packNeutral()
{
  m_roiData.resize(s_frames);
  int maxPin = 0;
  // RoI data
  for (int frame = 0; frame < s_frames; ++frame) {
    const LVL1::JEMTobRoI& roi(m_roiData[frame]);
    const int pin1 = frame / s_framesPerPin;
    const int pin2 = s_bunchCrossingPin + pin1 + 1;
    packerNeutral(pin1, roi.energyLarge(), s_energyLargeBits);
    packerNeutral(pin1, 0, 1);
    packerNeutral(pin2, roi.energySmall(), s_energySmallBits);
    packerNeutral(pin2, roi.location(), s_locationBits);
    maxPin = pin2;
  }
  // Bunch Crossing number
  packerNeutral(s_bunchCrossingPin, bunchCrossing(), s_bunchCrossingBits);
  packerNeutral(s_bunchCrossingPin, 0, s_paddingBits);
  // G-Link parity
  for (int pin = 0; pin <= maxPin; ++pin) packerNeutralParity(pin);

  return true;
}

// Unpack neutral data

bool JemRoiSubBlockV2::unpackNeutral()
{
  m_roiData.resize(s_frames);
  int maxPin  = 0;
  // RoI data
  for (int frame = 0; frame < s_frames; ++frame) {
    const int pin1 = frame / s_framesPerPin;
    const int pin2 = s_bunchCrossingPin + pin1 + 1;
    const int enLarge = unpackerNeutral(pin1, s_energyLargeBits);
    unpackerNeutral(pin1, 1);
    const int enSmall = unpackerNeutral(pin2, s_energySmallBits);
    const int loc     = unpackerNeutral(pin2, s_locationBits);
    m_roiData[frame] = LVL1::JEMTobRoI(crate(), module(),
                                       frame, loc, enLarge, enSmall);
    maxPin = pin2;
  }
  // Bunch Crossing number
  setBunchCrossing(unpackerNeutral(s_bunchCrossingPin, s_bunchCrossingBits));
  unpackerNeutral(s_bunchCrossingPin, s_paddingBits);
  // G-Link parity
  for (int pin = 0; pin <= maxPin; ++pin) unpackerNeutralParityError(pin);
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

} // end namespace
