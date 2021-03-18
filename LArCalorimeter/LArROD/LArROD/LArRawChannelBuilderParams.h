/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderParams
 * @author Denis Damazio
 * @brief Stores parameters for the current reconstructed LArRawChannel 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERPARAMS_H
#define LARROD_LARRAWCHANNELBUILDERPARAMS_H

#include "CaloIdentifier/CaloGain.h"

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "LArRawEvent/LArRawChannelContainer.h"

typedef struct {

  Identifier   curr_id;
  HWIdentifier curr_chid;
  CaloGain::CaloGain curr_gain;
  
           int curr_sample0 = 0;
           int curr_maximum = 0;
  unsigned int curr_maxsample = 0;
  unsigned int curr_nsamples = 0;
           int curr_shiftTimeSamples = 0;
	   int curr_Phase = 0;
  
  unsigned int qualityBitPattern = 0;
  LArRawChannelContainer* m_larRawChannelContainer = nullptr;

} LArRawChannelBuilderParams;

#endif
