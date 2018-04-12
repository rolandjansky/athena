/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArHVScaleCorr2Ntuple.h"
//#include "LArRawConditions/LArHVScaleCorrComplete.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "CaloIdentifier/CaloGain.h"

LArHVScaleCorr2Ntuple::LArHVScaleCorr2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) { 
  m_contKey = "LArHVScaleCorr";
  declareProperty("ContainerKey",m_contKey);

  m_ntTitle="HV Scale Correction";
  m_ntpath="/NTUPLES/FILE1/HVSCALE";

}

LArHVScaleCorr2Ntuple::~LArHVScaleCorr2Ntuple() 
{}

StatusCode LArHVScaleCorr2Ntuple::stop() {
 
  const ILArHVScaleCorr* larHVScaleCorr;
  StatusCode sc=m_detStore->retrieve(larHVScaleCorr,m_contKey);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Unable to retrieve ILArHVScaleCorr with key " 
	      << m_contKey << " from DetectorStore" );
    return StatusCode::FAILURE;
    } 

 NTuple::Item<float> corr;

 sc=m_nt->addItem("hvcorr",corr,-1000.,2.);
 if (sc!=StatusCode::SUCCESS)
   {ATH_MSG_ERROR( "addItem 'corr' failed" );
    return StatusCode::FAILURE;
   }


 unsigned cellCounter=0;
 std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
 std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
 for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     if (m_larCablingSvc->isOnlineConnected(hwid)) {
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
