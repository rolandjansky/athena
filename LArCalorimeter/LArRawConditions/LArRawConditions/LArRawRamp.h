/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWRAMP_H
#define LARRAWRAMP_H

#include <stdint.h>
#include <vector>
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloGain.h"


/** This class stores a unfittet ramp
    (=a vector a ADC and DAC values)
 *
 * @author W. Lampl <walter.lampl@cern.ch>
 * 26/02/04
 * Modifications:
 *    - 6/10/04, S. Laplace: added index of max. sample as data member
 *    - 7/03/05, S. Laplace: added averaged samples and RMS
 */

class LArRawRamp 
{
 public:

  typedef double RAMPDATA_t;

  class RAMPPOINT_t {
  public:
    RAMPDATA_t ADC;
    uint32_t DAC;
    float    iMaxSample;
    float    TimeMax;
    std::vector<float> Samples;
    std::vector<float> RMS;
    uint16_t NTriggers;
    bool operator<(const RAMPPOINT_t& a) const
    {return DAC<a.DAC;}  //to allow sorting of the vector
  RAMPPOINT_t() : ADC(0), DAC(0), iMaxSample(0),TimeMax(0),NTriggers(0) {};
  };
  
  LArRawRamp(HWIdentifier id, CaloGain::CaloGain gain);

  void add(const RAMPPOINT_t& point);
  void setsat(int iSat);

  inline HWIdentifier channelID() const
    {return m_ID;}
  inline CaloGain::CaloGain gain() const
    {return m_gain;}

  inline const std::vector<RAMPPOINT_t>& theRamp() const
    {return m_ramp;}
  inline std::vector<RAMPPOINT_t>& theRamp()
    {return m_ramp;}
  inline int iSat() const
    {return m_iSat;}

 private:
  std::vector<RAMPPOINT_t> m_ramp;
  HWIdentifier m_ID;
  CaloGain::CaloGain m_gain;
  int m_iSat;
};

#endif 
