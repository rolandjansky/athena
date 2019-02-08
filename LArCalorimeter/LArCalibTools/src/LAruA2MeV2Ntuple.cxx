/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LAruA2MeV2Ntuple.h"
#include "LArElecCalib/ILAruA2MeV.h"
#include "LArElecCalib/ILArDAC2uA.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"

LAruA2MeV2Ntuple::LAruA2MeV2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): LArCond2NtupleBase(name, pSvcLocator) { 
  declareProperty("uA2MeVKey",m_uA2MeVKey);
  declareProperty("DAC2uAKey",m_DAC2uAKey);

  m_ntTitle="ADC to Mev Conversion";
  m_ntpath="/NTUPLES/FILE1/ADCMEV";

}

LAruA2MeV2Ntuple::~LAruA2MeV2Ntuple() 
{}

StatusCode LAruA2MeV2Ntuple::stop() {
  StatusCode sc;
  NTuple::Item<long> cellIndex;
  NTuple::Item<float> uA2MeV;
  NTuple::Item<float> DAC2uA;
 
 sc=m_nt->addItem("icell",cellIndex,0,2000);
 if (sc!=StatusCode::SUCCESS)
   {(*m_log) << MSG::ERROR << "addItem 'Cell Index' failed" << endmsg;
    return StatusCode::FAILURE;
   }
 sc=m_nt->addItem("uAMeV",uA2MeV,-1000.,5000.);
 if (sc!=StatusCode::SUCCESS)
   {(*m_log) << MSG::ERROR << "addItem 'uAMeV' failed" << endmsg;
    return StatusCode::FAILURE;
   }

 sc=m_nt->addItem("DAC2uA",DAC2uA,-1000.,5000.);
 if (sc!=StatusCode::SUCCESS)
   {(*m_log) << MSG::ERROR << "addItem 'DAC2uA' failed" << endmsg;
    return StatusCode::FAILURE;
   }

 //const LAruA2MeVComplete* laruA2MeVComplete;
 const ILAruA2MeV* laruA2MeVComplete;
 sc=m_detStore->retrieve(laruA2MeVComplete,m_uA2MeVKey);
 if (sc!=StatusCode::SUCCESS)
   {(*m_log) << MSG::ERROR << "Unable to retrieve LAruA2MeVComplete with key " 
	<< m_uA2MeVKey << " from DetectorStore" << endmsg;
   return StatusCode::FAILURE;
   } 

 const ILArDAC2uA* larDAC2uAComplete;
 sc=m_detStore->retrieve(larDAC2uAComplete,m_DAC2uAKey);
 if (sc!=StatusCode::SUCCESS) {
   (*m_log) << MSG::ERROR << "Unable to retrieve LArDAC2uAComplete with key " 
	    << m_DAC2uAKey << " from DetectorStore" << endmsg;
   return StatusCode::FAILURE;
 } 

 SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
 const LArOnOffIdMapping* cabling=*cablingHdl;
 if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
 }

 std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
 std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
 unsigned cellCounter=0;
 for(; itOnId!=itOnIdEnd;++itOnId){
   const HWIdentifier hwid = *itOnId;
   if (cabling->isOnlineConnected(hwid)) {
     uA2MeV=laruA2MeVComplete->UA2MEV(hwid);
     if(m_DAC2uAKey != "") DAC2uA=larDAC2uAComplete->DAC2UA(hwid);
     fillFromIdentifier(hwid);
     cellIndex=cellCounter;
     sc=ntupleSvc()->writeRecord(m_nt);

     if (sc!=StatusCode::SUCCESS) {
       (*m_log) << MSG::ERROR << "writeRecord failed" << endmsg;
       return StatusCode::FAILURE;
     }
     cellCounter++;
   }//end if connected
 }//end loop over online ID

 (*m_log) << MSG::INFO << "LAruA2MeV2Ntuple has finished." << endmsg;
 return StatusCode::SUCCESS;
}// end finalize-method.
   
