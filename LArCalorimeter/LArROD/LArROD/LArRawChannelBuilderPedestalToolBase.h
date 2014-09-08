/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderPedestalToolBase
 * @author Rolf Seuster
 * @brief Base Tool for Pedestal Obtention
 */

#ifndef LARROD_LARRAWCHANNELBUILDERPEDESTALTOOLBASE_H
#define LARROD_LARRAWCHANNELBUILDERPEDESTALTOOLBASE_H

#include "GaudiKernel/AlgTool.h"

#include "LArROD/LArRawChannelBuilderToolBaseClass.h"
#include "LArROD/ILArRawChannelBuilderPedestalToolBase.h"

#include "LArElecCalib/ILArPedestal.h"

class LArRawChannelBuilderPedestalToolBase : public LArRawChannelBuilderToolBaseClass,
					     public virtual ILArRawChannelBuilderPedestalToolBase
{
 public:
  
  LArRawChannelBuilderPedestalToolBase(const std::string& type,
				       const std::string& name,
				       const IInterface* parent);
  
  virtual StatusCode initTool()
    { return StatusCode::SUCCESS; };
  
  void initEventHidden()
    { initEvent(); };
  
  virtual bool pedestal(float& pedestal, MsgStream* pLog) =0;
  
};

#endif
