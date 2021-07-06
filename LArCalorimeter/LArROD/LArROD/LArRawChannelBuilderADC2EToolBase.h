/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderADC2EToolBase
 * @author Rolf Seuster
 * @brief Base Tool to Convert ADC to energy
 */

#ifndef LARROD_LARRAWCHANNELBUILDERADC2ETOOLBASE_H
#define LARROD_LARRAWCHANNELBUILDERADC2ETOOLBASE_H

#include "GaudiKernel/AlgTool.h"

#include "LArROD/LArRawChannelBuilderToolBaseClass.h"
#include "LArROD/ILArRawChannelBuilderADC2EToolBase.h"

#include <string>
#include <vector>

class LArRawChannelBuilderADC2EToolBase : public LArRawChannelBuilderToolBaseClass,
					  virtual public ILArRawChannelBuilderADC2EToolBase
{
 public:
  
  LArRawChannelBuilderADC2EToolBase(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  
  virtual bool ADC2E(const EventContext& ctx,
                     std::vector<float>& Ramps, MsgStream* pLog) =0;
  
  void initEventHidden()
    { initEvent(); };
  
};
#endif

