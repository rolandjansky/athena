/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  
           int curr_sample0;
           int curr_maximum;
  unsigned int curr_maxsample;
  unsigned int curr_nsamples;
           int curr_shiftTimeSamples;
	   int curr_Phase;
  
  unsigned int qualityBitPattern;
  LArRawChannelContainer* m_larRawChannelContainer;

} LArRawChannelBuilderParams;

#endif
