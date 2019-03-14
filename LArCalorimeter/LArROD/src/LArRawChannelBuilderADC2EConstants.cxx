/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderADC2EConstants.h"
#include "LArROD/LArRawChannelBuilderStatistics.h"

#include "CaloIdentifier/CaloCell_ID.h"
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
  m_helper = new LArRawChannelBuilderStatistics( 1,      // number of possible errors
					       0x10);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
}

StatusCode LArRawChannelBuilderADC2EConstants::initTool()
{
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();
  m_fcalId=idHelper->fcal_idHelper();
  m_hecId=idHelper->hec_idHelper();
  
  return StatusCode::SUCCESS;
}

bool
LArRawChannelBuilderADC2EConstants::ADC2E(std::vector<float>& Ramps, MsgStream* pLog)
{
  Ramps.resize(2, 0);
  Ramps[0]=0;
  Ramps[1]=1;
  
  float GainFactor;
  if( m_parent->curr_gain == CaloGain::LARLOWGAIN ) {
    GainFactor = 9.8*9.8;
  } else if (m_parent->curr_gain == CaloGain::LARMEDIUMGAIN ) {
    GainFactor = 9.8;
  } else if (m_parent->curr_gain == CaloGain::LARHIGHGAIN ) {
    GainFactor = 1.0;
  } else {
    GainFactor = 1.0;
    if(bool(pLog))
      (*pLog) << MSG::WARNING << "Channel " << std::hex << m_parent->curr_chid << std::dec
	   << "unknown gain: " << m_parent->curr_gain << endmsg;
  }
  Ramps[1]*=GainFactor;
  
  Identifier id=currentID();
  
  float ADCtoMeVFCAL[3];
  float ADCtoMeVHEC[2];
  float ADCtoMeVEMECInner[2];
  float ADCtoMeVEMECOuter[2];
  float ADCtoMeVEMB[4];
  ADCtoMeVFCAL[0]      =   87.0 * MeV;  // FCAL1 High gain
  ADCtoMeVFCAL[1]      =  117.0 * MeV;  // FCAL2 High gain
  ADCtoMeVFCAL[2]      =  193.0 * MeV;  // FCAL3 High gain
  ADCtoMeVHEC[0]       =  136.0 / 9.8 * MeV;  // HEC 1 Medium gain from Monika.
  ADCtoMeVHEC[1]       =  136.0 / 9.8 * MeV;  // HEC 2 Medium gain from Monika.
  ADCtoMeVEMECInner[0] =   20.0 * MeV;  // EMEC  High gain : fixed 18/8/2004 RMcP
  ADCtoMeVEMECInner[1] =   20.0 * MeV;  // EMEC  High gain : fixed 18/8/2004 RMcP
  ADCtoMeVEMECOuter[0] =   16.0 * MeV;  // EMEC  High gain from Monika, approximate
  ADCtoMeVEMECOuter[1] =   16.0 * MeV;  // EMEC  High gain from Monika, approximate
  ADCtoMeVEMB[0]       =   7.0  * MeV;  // EMB   High gain from Isabelle, approximate
  ADCtoMeVEMB[1]       =   2.5  * MeV;  // EMB   High gain from Isabelle, approximate
  ADCtoMeVEMB[2]       =   18.0 * MeV;  // EMB   High gain from Isabelle, approximate
  ADCtoMeVEMB[3]       =   9.0  * MeV;  // EMB   High gain from Isabelle, approximate
  
  if (m_emId->is_em_barrel(id)) {
    const int layer= m_emId->sampling(id);
    const int eta=m_emId->eta(id);
    Ramps[1] *= ADCtoMeVEMB[layer];
    if (layer==2 && eta<32)
      Ramps[1] *= (12./18.); //Correct for lead thickness
  } else if (m_emId->is_em_endcap_inner(id)) {
    Ramps[1] *= ADCtoMeVEMECInner[m_emId->sampling(id)-1];
  } else if (m_emId->is_em_endcap_outer(id)) {
    Ramps[1] *= ADCtoMeVEMECOuter[m_emId->sampling(id)-1];
  } else if (m_fcalId->is_lar_fcal(id)) {
    Ramps[1] *= ADCtoMeVFCAL[m_fcalId->module(id)-1];
  } else if (m_hecId->is_lar_hec(id)) {
    Ramps[1] *= ADCtoMeVHEC[0];
  }
  // (*pLog) << MSG::VERBOSE << "ADC2EConstants tool - energy : " << energy << endmsg;
  m_helper->incrementErrorCount(0);
  m_parent->qualityBitPattern |= m_helper->returnBitPattern();
  
  return true;
}

