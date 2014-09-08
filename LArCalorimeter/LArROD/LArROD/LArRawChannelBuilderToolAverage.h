/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolAverage
 * @author Rolf Seuster
 * @brief Obtain ADC_max by using an average over samples
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLAVERAGE_H
#define LARROD_LARRAWCHANNELBUILDERTOOLAVERAGE_H

#include "LArROD/LArRawChannelBuilderToolBase.h"

#include <string>

class LArDigit;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArOnlineID;

class LArRawChannelBuilderToolAverage : public LArRawChannelBuilderToolBase
{
 public:
  
  LArRawChannelBuilderToolAverage(const std::string& type,
				  const std::string& name,
				  const IInterface* parent);
  
  bool buildRawChannel(const LArDigit* digit,
		       float pedestal,
		       const std::vector<float>& ramps,
		       MsgStream* pLog);
  
  StatusCode initTool();
  
 private:
  
  int m_averageSamplesEM,m_averageSamplesHEC,m_averageSamplesFCAL;
  float m_averageScaleEM,m_averageScaleHEC,m_averageScaleFCAL;
  unsigned int m_nScan; 
  
  const LArEM_ID*   m_emId;
  const LArFCAL_ID* m_fcalId;
  const LArHEC_ID*  m_hecId;

  const LArOnlineID* m_onlineHelper;  

  LArRawChannelBuilderToolAverage (const LArRawChannelBuilderToolAverage&);
  LArRawChannelBuilderToolAverage& operator= (const LArRawChannelBuilderToolAverage&);
};

#endif
