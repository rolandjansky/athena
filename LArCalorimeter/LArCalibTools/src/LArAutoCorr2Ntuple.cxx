/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArAutoCorr2Ntuple.h"
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"

LArAutoCorr2Ntuple::LArAutoCorr2Ntuple(const std::string& name, ISvcLocator* pSvcLocator)
  : LArCond2NtupleBase(name, pSvcLocator)
{
  declareProperty("Nsamples",    m_nsamples=32);
  
  m_ntTitle="AutoCorrelation";
  m_ntpath="/NTUPLES/FILE1/AUTOCORR";

}

StatusCode LArAutoCorr2Ntuple::initialize() {
   ATH_CHECK(m_objKey.initialize());
   return LArCond2NtupleBase::initialize();
}

LArAutoCorr2Ntuple::~LArAutoCorr2Ntuple() 
{}

StatusCode LArAutoCorr2Ntuple::stop() {
  
 ATH_MSG_INFO( "LArAutoCorr2Ntuple in stop " << m_nt);

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

 ATH_MSG_DEBUG( "LArAutoCorr2Ntuple reading container " << m_objKey.key());
 // For compatibility with existing configurations, look in the detector
 // store first, then in conditions.
 const ILArAutoCorr* larAutoCorr=nullptr;
 larAutoCorr = detStore()->tryConstRetrieve<ILArAutoCorr>(m_objKey.key());
 if(!larAutoCorr) {
    ATH_MSG_DEBUG( "No ILArAutoCorr found, trying LArAutoCorrComplete " << m_objKey.key());
    const LArAutoCorrComplete *larComplete =
        detStore()->tryConstRetrieve<LArAutoCorrComplete>(m_objKey.key());
    if(larComplete) {
       larAutoCorr = larComplete;
    } else {
       ATH_MSG_DEBUG( "LArAutoCorr2Ntuple reading conditions" );
       SG::ReadCondHandle<ILArAutoCorr> acHdl{m_objKey};
       larAutoCorr = *acHdl;
    }
 }
 if(larAutoCorr==nullptr){
   ATH_MSG_ERROR( "Unable to retrieve ILArAutoCorr with key " << m_objKey.key() << " neither from DetectorStore neither from conditions" );
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
   
