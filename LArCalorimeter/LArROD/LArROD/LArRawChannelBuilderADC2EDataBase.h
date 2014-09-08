/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderADC2EDataBase
 * @author Rolf Seuster
 * @brief Convert ADC to energy using the ADC2MeVTool, i.e. the databse
 */

#ifndef LARROD_LARRAWCHANNELBUILDERADC2EDATABASE_H
#define LARROD_LARRAWCHANNELBUILDERADC2EDATABASE_H

#include "GaudiKernel/ToolHandle.h"

#include "LArROD/LArRawChannelBuilderADC2EToolBase.h"
#include "LArElecCalib/ILArADC2MeVTool.h"

class LArOnlineID;

class LArRawChannelBuilderADC2EDataBase
: public LArRawChannelBuilderADC2EToolBase
{
 public:
  
  LArRawChannelBuilderADC2EDataBase(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  
  bool ADC2E(std::vector<float>& Ramps, MsgStream* pLog);
  
  StatusCode initTool();
  
 private:
  
  ToolHandle<ILArADC2MeVTool> m_adc2mevTool;
  
  bool m_testRamps;
  float m_ramp_max_high;
  float m_ramp_max_medium;
  float m_ramp_max_low;
  
  bool m_useIntercept_high;
  bool m_useIntercept_medium;
  bool m_useIntercept_low;

  const LArOnlineID_Base* m_onlineHelper ;
};

#endif
