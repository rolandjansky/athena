/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CaloEvent/JEMRoI.h"

#include "JemRoiSubBlockV1.h"

namespace LVL1BS {

// Constant definitions

const int JemRoiSubBlockV1::s_wordIdVal;

const int JemRoiSubBlockV1::s_frames;
const int JemRoiSubBlockV1::s_framesPerPin;
const int JemRoiSubBlockV1::s_bunchCrossingPin;
const int JemRoiSubBlockV1::s_hitsBits;
const int JemRoiSubBlockV1::s_locationBits;
const int JemRoiSubBlockV1::s_saturationBits;
const int JemRoiSubBlockV1::s_bunchCrossingBits;
const int JemRoiSubBlockV1::s_paddingBits;


JemRoiSubBlockV1::JemRoiSubBlockV1()
{
}

JemRoiSubBlockV1::~JemRoiSubBlockV1()
{
}

// Clear all data

void JemRoiSubBlockV1::clear()
{
  L1CaloSubBlock::clear();
  m_roiData.clear();
}

// Store header

void JemRoiSubBlockV1::setRoiHeader(const int version, const int crate,
                                                       const int module)
{
  setHeader(s_wordIdVal, version, NEUTRAL, 0, crate, module, 0, 1);
}

// Store RoI

void JemRoiSubBlockV1::fillRoi(const LVL1::JEMRoI roi)
{
  const LVL1::JEMRoI roiTemp(crate(), module(), 0, 0, 0, 0, 0);
  if (roi.crate() == roiTemp.crate() && roi.jem() == roiTemp.jem()) {
    m_roiData.resize(2*s_frames);
    const int pos = roi.frame() + roi.forward()*s_frames;
    m_roiData[pos] = roi;
  }
}

// Return RoI for given frame and forward

LVL1::JEMRoI JemRoiSubBlockV1::roi(const int frame, const int forward) const
{
  const int pos = frame + forward*s_frames;
  if (pos >= 0 && pos < 2*s_frames && !m_roiData.empty()) {
    return m_roiData[pos];
  } else return LVL1::JEMRoI(0);
}

// Packing/Unpacking routines

bool JemRoiSubBlockV1::pack()
{
  bool rc = false;
  switch (version()) {
    case 1:
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

bool JemRoiSubBlockV1::unpack()
{
  bool rc = false;
  switch (version()) {
    case 1:
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

bool JemRoiSubBlockV1::packNeutral()
{
  m_roiData.resize(2*s_frames);
  int maxPin = 0;
  // RoI data
  for (int pos = 0; pos < 2*s_frames; ++pos) {
    const LVL1::JEMRoI& roi(m_roiData[pos]);
    int pin = pos/s_framesPerPin;
    if (pin >= s_bunchCrossingPin) ++pin; // forward rois
    packerNeutral(pin, roi.hits(),     s_hitsBits);
    packerNeutral(pin, roi.error(),    s_saturationBits);
    packerNeutral(pin, roi.location(), s_locationBits);
    maxPin = pin;
  }
  // Bunch Crossing number
  packerNeutral(s_bunchCrossingPin, bunchCrossing(), s_bunchCrossingBits);
  packerNeutral(s_bunchCrossingPin, 0, s_paddingBits);
  // G-Link parity
  for (int pin = 0; pin <= maxPin; ++pin) packerNeutralParity(pin);

  return true;
}

// Unpack neutral data

bool JemRoiSubBlockV1::unpackNeutral()
{
  m_roiData.resize(2*s_frames);
  int maxPin  = 0;
  int forward = 0;
  int parity  = 0;
  // RoI data
  for (int pos = 0; pos < 2*s_frames; ++pos) {
    int pin = pos/s_framesPerPin;
    if (pin >= s_bunchCrossingPin) {
      ++pin;
      forward = 1;
    }
    const int hits = unpackerNeutral(pin, s_hitsBits);
    const int sat  = unpackerNeutral(pin, s_saturationBits);
    const int loc  = unpackerNeutral(pin, s_locationBits);
    const int err  = (parity << 1) | sat;
    m_roiData[pos] = LVL1::JEMRoI(crate(), module(), pos%s_frames, loc,
                                           forward, hits, err);
    maxPin = pin;
  }
  // Bunch Crossing number
  setBunchCrossing(unpackerNeutral(s_bunchCrossingPin, s_bunchCrossingBits));
  unpackerNeutral(s_bunchCrossingPin, s_paddingBits);
  // G-Link parity
  parity = 1;
  for (int pin = 0; pin <= maxPin; ++pin) {
    const bool error = unpackerNeutralParityError(pin);
    if (pin == s_bunchCrossingPin) continue;
    if (error) {
      for (int frame = 0; frame < s_framesPerPin; ++frame) {
	int pos = pin * s_framesPerPin + frame;
	if (pin > s_bunchCrossingPin) pos -= s_framesPerPin;
        const LVL1::JEMRoI roi = m_roiData[pos];
	const int err = (parity << 1) | roi.error();
	m_roiData[pos] = LVL1::JEMRoI(roi.crate(), roi.jem(), roi.frame(),
	                              roi.location(), roi.forward(),
			              roi.hits(), err);
      }
    }
  }
  const bool rc = unpackerSuccess();
  if (!rc) setUnpackErrorCode(UNPACK_DATA_TRUNCATED);
  return rc;
}

} // end namespace
