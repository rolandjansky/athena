/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file LArDSPThresholdsComplete.h
 * @brief A LArRawConditionsContainer holding thresholds used by the DSP
 * @author Walter Lampl  <walter.lampl@cern.ch>
 *
 * $Id: LArDSPThresholdsComplete.h,v 1.1 2008-10-08 10:01:11 wlampl Exp $
 */


#ifndef LARDSPTHRESHOLDSCOMPLETE_H
#define LARDSPTHRESHOLDSCOMPLETE_H

#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArDSPThresholdsP.h"

class LArDSPThresholdsComplete : public LArConditionsContainer<LArDSPThresholdsP> {

 public:
  void set(const HWIdentifier chid, const float tQThr, const float samplesThr, const float trigSumThr); 

  float tQThr(const HWIdentifier chid) const { return this->get(chid,0).tQThr(); }; 
  float samplesThr(const HWIdentifier chid) const {return this->get(chid,0).samplesThr();}; 
  float trigSumThr(const HWIdentifier chid) const {return this->get(chid,0).trigSumThr();}; 

};


CLASS_DEF(LArDSPThresholdsComplete,247471531,1)

#endif
