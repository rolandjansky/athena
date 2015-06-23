/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderADC2EConstants.h"
#include "LArROD/LArRawChannelBuilderStatistics.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "Identifier/Identifier.h"

using CLHEP::MeV;

LArRawChannelBuilderADC2EConstants::LArRawChannelBuilderADC2EConstants(const std::string& type,
								       const std::string& name,
								       const IInterface* parent):
  LArRawChannelBuilderADC2EToolBase(type,name,parent),
  m_emId(0),
  m_fcalId(0),
  m_hecId(0)
{
  helper = new LArRawChannelBuilderStatistics( 1,      // number of possible errors
					       0x10);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  helper->setErrorString(0, "no errors");
}

StatusCode LArRawChannelBuilderADC2EConstants::initTool()
{
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  m_fcalId=caloIdMgr->getFCAL_ID();
  m_hecId=caloIdMgr->getHEC_ID();
  
  return StatusCode::SUCCESS;
}

bool
LArRawChannelBuilderADC2EConstants::ADC2E(std::vector<float>& Ramps, MsgStream* pLog)
{
  Ramps.resize(2, 0);
  Ramps[0]=0;
  Ramps[1]=1;
  
  float GainFactor;
  if( pParent->curr_gain == CaloGain::LARLOWGAIN ) {
    GainFactor = 9.8*9.8;
  } else if (pParent->curr_gain == CaloGain::LARMEDIUMGAIN ) {
    GainFactor = 9.8;
  } else if (pParent->curr_gain == CaloGain::LARHIGHGAIN ) {
    GainFactor = 1.0;
  } else {
    GainFactor = 1.0;
    if(bool(pLog))
      (*pLog) << MSG::WARNING << "Channel " << std::hex << pParent->curr_chid << std::dec
	   << "unknown gain: " << pParent->curr_gain << endreq;
  }
  Ramps[1]*=GainFactor;
  
  Identifier id=currentID();
  
  float m_ADCtoMeVFCAL[3];
  float m_ADCtoMeVHEC[2];
  float m_ADCtoMeVEMECInner[2];
  float m_ADCtoMeVEMECOuter[2];
  float m_ADCtoMeVEMB[4];
  m_ADCtoMeVFCAL[0]      =   87.0 * MeV;  // FCAL1 High gain
  m_ADCtoMeVFCAL[1]      =  117.0 * MeV;  // FCAL2 High gain
  m_ADCtoMeVFCAL[2]      =  193.0 * MeV;  // FCAL3 High gain
  m_ADCtoMeVHEC[0]       =  136.0 / 9.8 * MeV;  // HEC 1 Medium gain from Monika.
  m_ADCtoMeVHEC[1]       =  136.0 / 9.8 * MeV;  // HEC 2 Medium gain from Monika.
  m_ADCtoMeVEMECInner[0] =   20.0 * MeV;  // EMEC  High gain : fixed 18/8/2004 RMcP
  m_ADCtoMeVEMECInner[1] =   20.0 * MeV;  // EMEC  High gain : fixed 18/8/2004 RMcP
  m_ADCtoMeVEMECOuter[0] =   16.0 * MeV;  // EMEC  High gain from Monika, approximate
  m_ADCtoMeVEMECOuter[1] =   16.0 * MeV;  // EMEC  High gain from Monika, approximate
  m_ADCtoMeVEMB[0]       =   7.0  * MeV;  // EMB   High gain from Isabelle, approximate
  m_ADCtoMeVEMB[1]       =   2.5  * MeV;  // EMB   High gain from Isabelle, approximate
  m_ADCtoMeVEMB[2]       =   18.0 * MeV;  // EMB   High gain from Isabelle, approximate
  m_ADCtoMeVEMB[3]       =   9.0  * MeV;  // EMB   High gain from Isabelle, approximate
  
  if (m_emId->is_em_barrel(id)) {
    const int layer= m_emId->sampling(id);
    const int eta=m_emId->eta(id);
    Ramps[1] *= m_ADCtoMeVEMB[layer];
    if (layer==2 && eta<32)
      Ramps[1] *= (12./18.); //Correct for lead thickness
  } else if (m_emId->is_em_endcap_inner(id)) {
    Ramps[1] *= m_ADCtoMeVEMECInner[m_emId->sampling(id)-1];
  } else if (m_emId->is_em_endcap_outer(id)) {
    Ramps[1] *= m_ADCtoMeVEMECOuter[m_emId->sampling(id)-1];
  } else if (m_fcalId->is_lar_fcal(id)) {
    Ramps[1] *= m_ADCtoMeVFCAL[m_fcalId->module(id)-1];
  } else if (m_hecId->is_lar_hec(id)) {
    Ramps[1] *= m_ADCtoMeVHEC[0];
  }
  // (*pLog) << MSG::VERBOSE << "ADC2EConstants tool - energy : " << energy << endreq;
  helper->incrementErrorCount(0);
  pParent->qualityBitPattern |= helper->returnBitPattern();
  
  return true;
}

