/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArMphysOverMcal2Ntuple.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "LArRawConditions/LArMphysOverMcalMC.h"
#include "CaloIdentifier/CaloGain.h"
/*
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
*/

//#include <fstream>

LArMphysOverMcal2Ntuple::LArMphysOverMcal2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) { 
  declareProperty("ContainerKey",m_contKey);
  declareProperty("IsMC",m_isMC = false);

  m_ntTitle="MphysOverMcal";
  m_ntpath="/NTUPLES/FILE1/MPMC";

}

LArMphysOverMcal2Ntuple::~LArMphysOverMcal2Ntuple() 
{}

StatusCode LArMphysOverMcal2Ntuple::stop() {
  const ILArMphysOverMcal* larMphysOverMcal = NULL;
  StatusCode sc;
  sc=m_detStore->retrieve(larMphysOverMcal,m_contKey);
  if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "Unable to retrieve ILArMphysOverMcal with key " 
               << m_contKey << " from DetectorStore" );
     return StatusCode::FAILURE;
  }

 NTuple::Item<long> cellIndex,gain;
 NTuple::Item<float> mpmc;

 sc=m_nt->addItem("icell",cellIndex,0,2000);
 if (sc!=StatusCode::SUCCESS)
   {ATH_MSG_ERROR( "addItem 'Cell Index' failed" );
    return StatusCode::FAILURE;
   }

 sc=m_nt->addItem("gain",gain,0,3);
 if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'gain' failed" );
    return StatusCode::FAILURE;
 }


 sc=m_nt->addItem("mphysovermcal",mpmc,-1000.,2.);
 if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'mphysovermcal' failed" );
    return StatusCode::FAILURE;
 }


 unsigned cellCounter=0;
 for(long igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++) {
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     if ( m_larCablingSvc->isOnlineConnected(hwid) && !m_onlineId->isFCALchannel(hwid)) {
	 fillFromIdentifier(hwid);       
	 cellIndex = cellCounter;
         gain=igain;     
	 mpmc = larMphysOverMcal->MphysOverMcal(hwid,igain);
	 sc=ntupleSvc()->writeRecord(m_nt);
	 if (sc!=StatusCode::SUCCESS) {
	   ATH_MSG_ERROR( "writeRecord failed" );
	   return StatusCode::FAILURE;
	 }
     }//end if isConnected
     cellCounter++;
  }//end loop over online ID
 } // ovr gains

 ATH_MSG_INFO( "LArMphysOverMcal2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
   
