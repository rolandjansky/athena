/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArOFCBin2Ntuple.h"
#include "LArRawConditions/LArRinjComplete.h"

#include "LArIdentifier/LArOnlineID.h"


LArOFCBin2Ntuple::LArOFCBin2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) { 
  m_ntTitle="OFCBIN";
  m_ntpath="/NTUPLES/FILE1/OFCBIN";

}

StatusCode LArOFCBin2Ntuple::initialize() 
{
  ATH_CHECK( m_binKey.initialize() );
  ATH_CHECK( m_cablingKey.initialize(!m_isSC) );
  ATH_CHECK( m_cablingSCKey.initialize(m_isSC) );

  ATH_CHECK( LArCond2NtupleBase::initialize() );
  return StatusCode::SUCCESS;
}

StatusCode LArOFCBin2Ntuple::stop() {

  const EventContext& ctx = Gaudi::Hive::currentContext();

 NTuple::Item<long> cellIndex;
 NTuple::Item<int> ofcbin;

 StatusCode sc=m_nt->addItem("icell",cellIndex,0,2000);
 if (sc!=StatusCode::SUCCESS)
   {ATH_MSG_ERROR( "addItem 'Cell Index' failed" );
    return StatusCode::FAILURE;
   }


 sc=m_nt->addItem("OFCBin",ofcbin,0,50);
 if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'OFCBin' failed" );
    return StatusCode::FAILURE;
 }

  const LArOnOffIdMapping *cabling=0;
  if(m_isSC) {
    ATH_MSG_DEBUG( "LArOFC2Ntuple: using SC cabling" );
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingSCKey, ctx};
    cabling=*cablingHdl;
  }else{
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
    cabling=*cablingHdl;
  }


  if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
  }

  const ILArOFCBin* LArbin = nullptr;
  SG::ReadCondHandle<ILArOFCBin> ofcbinHdl(m_binKey, ctx);
  LArbin = ofcbinHdl.cptr();
  if(!LArbin) {
        ATH_MSG_ERROR( "Unable to retrieve ILArOFCBin with key " 
               << m_binKey << " from ConditionsStore" );
        return StatusCode::FAILURE;
  }

 unsigned cellCounter=0;
 for(HWIdentifier hwid : m_onlineId->channel_range()){
   if ( cabling->isOnlineConnected(hwid)) {
       fillFromIdentifier(hwid);       
       cellIndex = cellCounter;
       ofcbin = LArbin->bin(hwid,0);
       sc=ntupleSvc()->writeRecord(m_nt);
       if (sc!=StatusCode::SUCCESS) {
         ATH_MSG_ERROR( "writeRecord failed" );
         return StatusCode::FAILURE;
       }
       cellCounter++;
   }//end if isConnected
 }//end loop over online ID
 ATH_MSG_INFO("LArOFCBin2Ntuple: filled "<<cellCounter);
 ATH_MSG_INFO( "LArOFCBin2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
   
