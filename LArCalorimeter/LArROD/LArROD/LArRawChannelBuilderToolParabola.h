/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolParabola
 * @author Rolf Seuster
 * @brief Obtain ADC_max by making a parabolic fit on the samples
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLPARABOLA_H
#define LARROD_LARRAWCHANNELBUILDERTOOLPARABOLA_H

#include "GaudiKernel/ToolHandle.h"

#include "LArROD/LArRawChannelBuilderToolBase.h"

#include "LArRecUtils/LArParabolaPeakRecoTool.h"

class LArEM_ID;
class LArDigit;

class LArRawChannelBuilderToolParabola : public LArRawChannelBuilderToolBase
{
 public:
  
  LArRawChannelBuilderToolParabola(const std::string& type,
				   const std::string& name,
				   const IInterface* parent);
  StatusCode initTool();
  
  bool buildRawChannel(const LArDigit* digit,
		       float pedestal,
		       const std::vector<float>& ramps,
		       MsgStream* pLog);
  
 private:
  
  ToolHandle<LArParabolaPeakRecoTool> m_parabolaRecoTool;
  
  const LArEM_ID*   m_emId;
  
  LArRawChannelBuilderToolParabola (const LArRawChannelBuilderToolParabola&);
  LArRawChannelBuilderToolParabola& operator= (const LArRawChannelBuilderToolParabola&);
};

#endif
