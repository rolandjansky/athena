/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArfSampl2Ntuple.h"
#include "LArRawConditions/LArfSamplMC.h"
#include "CaloIdentifier/CaloGain.h"
/*
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
*/

//#include <fstream>

LArfSampl2Ntuple::LArfSampl2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) { 
  declareProperty("ContainerKey",m_contKey);
  //declareProperty("IsMC",m_isMC = false);

  m_ntTitle="fSampl";
  m_ntpath="/NTUPLES/FILE1/FSAMPL";

}

LArfSampl2Ntuple::~LArfSampl2Ntuple() 
{}

StatusCode LArfSampl2Ntuple::stop() {
  const ILArfSampl* larfSampl = NULL;
  StatusCode sc;
  sc=m_detStore->retrieve(larfSampl,m_contKey);
  if (sc!=StatusCode::SUCCESS) {
     (*m_log)  << MSG::ERROR << "Unable to retrieve ILArNoise with key " 
               << m_contKey << " from DetectorStore" << endmsg;
     return StatusCode::FAILURE;
  }

 NTuple::Item<long> cellIndex;
 NTuple::Item<float> fsampl;

 sc=m_nt->addItem("icell",cellIndex,0,2000);
 if (sc!=StatusCode::SUCCESS)
   {(*m_log)  << MSG::ERROR << "addItem 'Cell Index' failed" << endmsg;
    return StatusCode::FAILURE;
   }

 sc=m_nt->addItem("fsampl",fsampl);
 if (sc!=StatusCode::SUCCESS)
   {(*m_log)  << MSG::ERROR << "addItem 'fsampl' failed" << endmsg;
    return StatusCode::FAILURE;
   }


 unsigned cellCounter=0;
 std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
 std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
 for(; itOnId!=itOnIdEnd;++itOnId){
   const HWIdentifier hwid = *itOnId;
   if ( m_larCablingSvc->isOnlineConnected(hwid)) {
       fillFromIdentifier(hwid);       
       cellIndex = cellCounter;
       fsampl = larfSampl->FSAMPL(hwid);
       sc=ntupleSvc()->writeRecord(m_nt);
       if (sc!=StatusCode::SUCCESS) {
         (*m_log)  << MSG::ERROR << "writeRecord failed" << endmsg;
         return StatusCode::FAILURE;
       }
   }//end if isConnected
   cellCounter++;
 }//end loop over online ID

 (*m_log)  << MSG::INFO << "LArfSampl2Ntuple has finished." << endmsg;
 return StatusCode::SUCCESS;
}// end finalize-method.
   
