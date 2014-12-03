/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderADC2EConstants
 * @author Rolf Seuster
 * @brief Convert ADC to energy using hardcoded calibration constants
 */

#ifndef LARROD_LARRAWCHANNELBUILDERADC2ECONSTANTS_H
#define LARROD_LARRAWCHANNELBUILDERADC2ECONSTANTS_H

#include "LArROD/LArRawChannelBuilderADC2EToolBase.h"

#include "CLHEP/Units/SystemOfUnits.h"

class LArEM_Base_ID;
class LArFCAL_Base_ID;
class LArHEC_Base_ID;

class LArRawChannelBuilderADC2EConstants
: public LArRawChannelBuilderADC2EToolBase
{
 public:
  
  LArRawChannelBuilderADC2EConstants(const std::string& type,
				     const std::string& name,
				     const IInterface* parent);
  
  bool ADC2E(std::vector<float>& Ramps, MsgStream* pLog);

  StatusCode initTool();
  
 private:
  
  const LArEM_Base_ID*   m_emId;
  const LArFCAL_Base_ID* m_fcalId;
  const LArHEC_Base_ID*  m_hecId;

  LArRawChannelBuilderADC2EConstants(const LArRawChannelBuilderADC2EConstants&);
  LArRawChannelBuilderADC2EConstants& operator=(const LArRawChannelBuilderADC2EConstants&);
};

#endif
