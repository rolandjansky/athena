/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArMphysOverMcal2Ntuple.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "LArRawConditions/LArMphysOverMcalMC.h"
#include "CaloIdentifier/CaloGain.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"


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


 const LArOnOffIdMapping *cabling=0;
 if(m_isSC) {
   ATH_MSG_DEBUG( "LArMphysOverMcal2Ntuple: using SC cabling" );
   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingSCKey};
   cabling=*cablingHdl;
 }else{
   ATH_MSG_DEBUG( "LArMphysOverMcal2Ntuple: using LAr cell cabling" );
   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
   cabling=*cablingHdl;
 }
 if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
 }

 //=================

 if(m_isSC){
   const LArOnline_SuperCellID* ll;
   sc = detStore()->retrieve(ll, "LArOnline_SuperCellID");
   if (sc.isFailure()) {
     msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
     return StatusCode::FAILURE;
   }
   else {
     m_onlineId = (const LArOnlineID_Base*)ll;
     ATH_MSG_DEBUG("Found the SC LArOnlineID helper");
   }
 }else{
   const LArOnlineID* ll;
   sc = detStore()->retrieve(ll, "LArOnlineID");
   if (sc.isFailure()) {
     msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
     return StatusCode::FAILURE;
   }
   else {
     m_onlineId = (const LArOnlineID_Base*)ll;
     ATH_MSG_DEBUG(" Found the LAr cell LArOnlineID helper. ");
   }
   
 }

 // ==============

 unsigned cellCounter=0;
 unsigned filledCell=0;
 for(long igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++) {
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     if ( cabling->isOnlineConnected(hwid) && !m_onlineId->isFCALchannel(hwid)) {
	 fillFromIdentifier(hwid);       
	 cellIndex = cellCounter;
         gain=igain;     
	 mpmc = larMphysOverMcal->MphysOverMcal(hwid,igain);
	 sc=ntupleSvc()->writeRecord(m_nt);
	 if (sc!=StatusCode::SUCCESS) {
	   ATH_MSG_ERROR( "writeRecord failed" );
	   return StatusCode::FAILURE;
	 }
	 filledCell++;
     }//end if isConnected
     cellCounter++;
  }//end loop over online ID
 } // ovr gains
 ATH_MSG_INFO("LArMphysOverMcal2Ntuple: filled "<<filledCell<<" out of "<<cellCounter);
 ATH_MSG_INFO( "LArMphysOverMcal2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
   
