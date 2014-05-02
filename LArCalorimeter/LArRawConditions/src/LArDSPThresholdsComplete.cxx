/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <limits>
#include "LArRawConditions/LArDSPThresholdsComplete.h"


//The default thresholds should be very high (eg. never go over threshold)
//We have to keep in mind that the data is casted to an int somewhere down the chain.
//This number shoudl we very high but can be safely casted to an int.
const float LArDSPThresholdsP::m_default=std::numeric_limits<int>::max()*0.99;


void LArDSPThresholdsComplete::set(const HWIdentifier chid, 
				   const float tQThr, const float samplesThr, 
				   const float trigSumThr) {
  LArDSPThresholdsP data(tQThr,samplesThr,trigSumThr);
  this->setPdata(chid,data,0);
}
