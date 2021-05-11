/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "LArRawConditions/LArADC2MeV.h"
#include "StoreGate/ReadCondHandleKey.h"

class LArOnlineID;

class LArRawChannelBuilderADC2EDataBase
: public LArRawChannelBuilderADC2EToolBase
{
 public:
  
  LArRawChannelBuilderADC2EDataBase(const std::string& type,
				    const std::string& name,
				    const IInterface* parent);
  
  virtual bool ADC2E(const EventContext& ctx,
                     std::vector<float>& Ramps, MsgStream* pLog) override;

  virtual StatusCode initialize() override;
  
  virtual StatusCode initTool() override;
  
 private:
  
  SG::ReadCondHandleKey<LArADC2MeV> m_adc2mevKey
  { this, "ADC2MeVKey", "LArADC2MeV", "SG Key of the LArADC2MeV CDO" };
  
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
