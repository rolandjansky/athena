/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAutoCorr2Ntuple.h"
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"

LArAutoCorr2Ntuple::LArAutoCorr2Ntuple(const std::string& name, ISvcLocator* pSvcLocator)
  : LArCond2NtupleBase(name, pSvcLocator),m_nsamples(7)
{
  declareProperty("ContainerKey",m_objKey);
  declareProperty("Nsamples",    m_nsamples);
  
  m_ntTitle="AutoCorrelation";
  m_ntpath="/NTUPLES/FILE1/AUTOCORR";

}


LArAutoCorr2Ntuple::~LArAutoCorr2Ntuple() 
{}

StatusCode LArAutoCorr2Ntuple::stop() {
  
 StatusCode sc; 
 NTuple::Array<float> cov;
 NTuple::Item<long> gain, cellIndex;
 sc=m_nt->addItem("gain",gain,0,3);
 if (sc!=StatusCode::SUCCESS) {
   ATH_MSG_ERROR( "addItem 'gain' failed" );
   return StatusCode::FAILURE;
 }

 sc=m_nt->addItem("cellIndex",cellIndex,0,200000);
 if (sc!=StatusCode::SUCCESS) {
   ATH_MSG_ERROR( "addItem 'cellIndex' failed" );
   return StatusCode::FAILURE;
 }

 sc=m_nt->addItem("covr",m_nsamples-1,cov);
 if (sc!=StatusCode::SUCCESS) {
   ATH_MSG_ERROR( "addItem 'covr' failed" );
   return StatusCode::FAILURE;
 }

 const ILArAutoCorr* larAutoCorr;
 sc=m_detStore->retrieve(larAutoCorr,m_objKey);
 if (sc!=StatusCode::SUCCESS){
   ATH_MSG_ERROR( "Unable to retrieve ILArAutoCorr with key " 
       << m_objKey << " from DetectorStore" );
   return StatusCode::FAILURE;
 } 


 unsigned cellCounter=0;
 unsigned cellZeroCounter=0;
 for ( unsigned igain=CaloGain::LARHIGHGAIN; 
       igain<CaloGain::LARNGAIN ; ++igain ) {
   std::vector<HWIdentifier>::const_iterator it = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator it_e = m_onlineId->channel_end();
   for (;it!=it_e;it++) {
     const HWIdentifier hwid = *it;
     ILArAutoCorr::AutoCorrRef_t corr=larAutoCorr->autoCorr(hwid,igain);
     if (corr.size()>0) {
       fillFromIdentifier(hwid); 
       gain = igain;
       cellIndex = cellCounter;
       for(unsigned i=0;i<m_nsamples-1 && i<corr.size();i++)
	 cov[i] = corr[i];

       sc = ntupleSvc()->writeRecord(m_nt);
       if (sc!=StatusCode::SUCCESS) {
	 ATH_MSG_ERROR( "writeRecord failed" );
	 return StatusCode::FAILURE;
       }
       cellCounter++;
     } else { ++cellZeroCounter;}//end if size>0
   }//end if loop over cells
 }//end if loop over gains
 
 ATH_MSG_INFO( "LArAutoCorr2Ntuple has finished, " << cellCounter << "records written, " << cellZeroCounter << " zero length vectors" );
 return StatusCode::SUCCESS;
}// end finalize-method.
   
