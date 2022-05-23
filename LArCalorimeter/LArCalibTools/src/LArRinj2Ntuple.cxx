/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArRinj2Ntuple.h"
#include "LArRawConditions/LArRinjComplete.h"

#include "LArIdentifier/LArOnlineID.h"


LArRinj2Ntuple::LArRinj2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) { 
  declareProperty("ContainerKey",m_contKey);
  m_ntTitle="Rinj";
  m_ntpath="/NTUPLES/FILE1/RINJ";

}

LArRinj2Ntuple::~LArRinj2Ntuple() 
{}

StatusCode LArRinj2Ntuple::stop() {
  const ILArRinj* LArRinj = NULL;
  StatusCode sc;
  sc=m_detStore->retrieve(LArRinj,m_contKey);
  if (sc!=StatusCode::SUCCESS) {
     ATH_MSG_ERROR( "Unable to retrieve ILArRinj with key " 
               << m_contKey << " from DetectorStore try ConditionsStore" );
     SG::ReadCondHandle<LArRinjComplete> rinjHdl{m_contKey};
     LArRinj = *rinjHdl;
     if(!LArRinj) {
        ATH_MSG_ERROR( "Unable to retrieve ILArRinj with key " 
               << m_contKey << " from ConditionsStore as well" );
        return StatusCode::FAILURE;
     }
  }

 NTuple::Item<long> cellIndex;
 NTuple::Item<float> rinj;

 sc=m_nt->addItem("icell",cellIndex,0,2000);
 if (sc!=StatusCode::SUCCESS)
   {ATH_MSG_ERROR( "addItem 'Cell Index' failed" );
    return StatusCode::FAILURE;
   }


 sc=m_nt->addItem("rinj",rinj,-1000.,2.);
 if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'rinj' failed" );
    return StatusCode::FAILURE;
 }


 ATH_MSG_DEBUG( "LArRinj2Ntuple: using LAr cell cabling" );
 SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
 const LArOnOffIdMapping *cabling=*cablingHdl;
 if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
 }

 //=================

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
   
 // ==============

 unsigned cellCounter=0;
 std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
 std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
 for(; itOnId!=itOnIdEnd;++itOnId){
   const HWIdentifier hwid = *itOnId;
   if ( cabling->isOnlineConnected(hwid)) {
       fillFromIdentifier(hwid);       
       cellIndex = cellCounter;
       rinj = LArRinj->Rinj(hwid);
       sc=ntupleSvc()->writeRecord(m_nt);
       if (sc!=StatusCode::SUCCESS) {
         ATH_MSG_ERROR( "writeRecord failed" );
         return StatusCode::FAILURE;
       }
       cellCounter++;
   }//end if isConnected
 }//end loop over online ID
 ATH_MSG_INFO("LArRinj2Ntuple: filled "<<cellCounter);
 ATH_MSG_INFO( "LArRinj2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
   
