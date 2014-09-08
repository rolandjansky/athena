/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolCubic
 * @author Rolf Seuster
 * @brief Obtain ADC_max by making a cubic fit on samples
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLCUBIC_H
#define LARROD_LARRAWCHANNELBUILDERTOOLCUBIC_H

#include "LArROD/LArRawChannelBuilderToolBase.h"

class LArDigit;
class LArFCAL_ID;

class LArRawChannelBuilderToolCubic : public LArRawChannelBuilderToolBase
{
 public:
  
  LArRawChannelBuilderToolCubic(const std::string& type,
				const std::string& name,
				const IInterface* parent);
  
  bool buildRawChannel(const LArDigit* digit,
		       float pedestal,
		       const std::vector<float>& ramps,
		       MsgStream* pLog);
  
  StatusCode initTool();
  
 private:
  
  bool m_useMaxSample ;
  const LArFCAL_ID* m_fcalId;
  int m_minADC; 

  LArRawChannelBuilderToolCubic (const LArRawChannelBuilderToolCubic&);
  LArRawChannelBuilderToolCubic& operator= (const LArRawChannelBuilderToolCubic&);
};

#endif
