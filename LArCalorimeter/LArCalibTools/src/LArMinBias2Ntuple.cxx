/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArMinBias2Ntuple.h"
#include "LArRawConditions/LArMinBiasMC.h"
#include "LArRawConditions/LArMinBiasAverageMC.h"
#include "CaloIdentifier/CaloGain.h"
/*
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
*/

//#include <fstream>

LArMinBias2Ntuple::LArMinBias2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator),
  m_isPileup(false) { 
  declareProperty("ContainerKey",m_contKey="LArMinBias");
  //declareProperty("IsMC",m_isMC = false);

  declareProperty("NtupleTitle",m_ntTitle="MinBias");
  declareProperty("NtupleName",m_ntpath="/NTUPLES/FILE1/MINBIAS");

}

LArMinBias2Ntuple::~LArMinBias2Ntuple() 
{}

StatusCode LArMinBias2Ntuple::stop() {
   
  const ILArMinBias* LArMinBias = NULL;
  const ILArMinBiasAverage* LArMinBiasAv = NULL;
  
  if(m_contKey.find("Pileup") != std::string::npos) m_isPileup=true; else m_isPileup=false;
  if(!m_isPileup) ATH_CHECK( m_detStore->retrieve(LArMinBias,m_contKey) );
  ATH_CHECK( m_detStore->retrieve(LArMinBiasAv,m_contKey+"Average") );

 NTuple::Item<float> minbias;
 NTuple::Item<float> minbias_av;


 if(!m_isPileup) ATH_CHECK( m_nt->addItem("MinBias",minbias) );
 ATH_CHECK( m_nt->addItem("MinBiasAv",minbias_av) );

 unsigned cellCounter=0;
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     if ( m_larCablingSvc->isOnlineConnected(hwid)) {
	 fillFromIdentifier(hwid);       
	 if(!m_isPileup) minbias = LArMinBias->minBiasRMS(hwid);
	 minbias_av = LArMinBiasAv->minBiasAverage(hwid);
	 ATH_CHECK( ntupleSvc()->writeRecord(m_nt) );
     }//end if isConnected
     cellCounter++;
  }//end loop over online ID

 ATH_MSG_INFO(  "LArMinBias2Ntuple has finished, " << cellCounter << " cells written." );
 return StatusCode::SUCCESS;
}// end finalize-method.
   
