/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArHVScaleCorr2Ntuple.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "CaloIdentifier/CaloGain.h"

LArHVScaleCorr2Ntuple::LArHVScaleCorr2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator)
{
  m_ntTitle="HV Scale Correction"; 
  m_ntpath="/NTUPLES/FILE1/HVSCALE";
}

StatusCode LArHVScaleCorr2Ntuple::initialize() {
   ATH_CHECK(m_contKey.initialize());
   return LArCond2NtupleBase::initialize();
}

StatusCode LArHVScaleCorr2Ntuple::stop() {
 
 const EventContext& ctx = Gaudi::Hive::currentContext();
 SG::ReadCondHandle<ILArHVScaleCorr> hvHdl(m_contKey, ctx);  
 const ILArHVScaleCorr* larHVScaleCorr = hvHdl.cptr();
 if(!larHVScaleCorr) {
    ATH_MSG_WARNING("Could not retrieve the ILArHVScaleCorr from CondStore with key: " << m_contKey.key());
    return StatusCode::SUCCESS;
 }

 NTuple::Item<float> corr;

 StatusCode sc=m_nt->addItem("hvcorr",corr,-1000.,2.);
 if (sc!=StatusCode::SUCCESS)
   {ATH_MSG_ERROR( "addItem 'corr' failed" );
    return StatusCode::FAILURE;
   }

 SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
 const LArOnOffIdMapping* cabling=*cablingHdl;
 if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
 }

 unsigned cellCounter=0;
 std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
 std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
 for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     if (cabling->isOnlineConnected(hwid)) {
       float value=larHVScaleCorr->HVScaleCorr(hwid);
       if (value > ILArHVScaleCorr::ERRORCODE) { // check for ERRORCODE
	 fillFromIdentifier(hwid);       
	 corr=value;
	 sc=ntupleSvc()->writeRecord(m_nt);
	 if (sc!=StatusCode::SUCCESS) {
	   ATH_MSG_ERROR( "writeRecord failed" );
	   return StatusCode::FAILURE;
	 }
       }// end if object exists
     }//end if isConnected
  cellCounter++;
 }//end loop over online ID

 ATH_MSG_INFO( "LArHVScaleCorr2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
