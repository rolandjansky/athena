/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolBadChannelTool
 * @author Rolf Seuster
 * @brief Check for bad channels using LArBadChannelTool and increment the error counter
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLBADCHANNELTOOL_H
#define LARROD_LARRAWCHANNELBUILDERTOOLBADCHANNELTOOL_H

#include "GaudiKernel/ToolHandle.h"

#include "LArROD/LArRawChannelBuilderToolBase.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>

class LArDigit;

class LArRawChannelBuilderToolBadChannelTool : public LArRawChannelBuilderToolBase
{
 public:
  
  LArRawChannelBuilderToolBadChannelTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent);
  
  bool buildRawChannel(const LArDigit* digit,
		       float pedestal,
		       const std::vector<float>& ramps,
		       MsgStream* pLog);
  
  StatusCode initTool();

 private:
  LArBadChannelMask m_bcMask;
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{},"Bad-Channel categories to mask"}; 
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};    
};

#endif
