/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArOFC2Ntuple.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"

LArOFC2Ntuple::LArOFC2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator)
{
  declareProperty("ContainerKey", m_contKey  = "LArOFC");
  //declareProperty("Nsamples",     m_nSamples = 5);
  //declareProperty("Nphases",      m_nPhases  = 50);
  declareProperty("NtupleName",   m_ntName   = "OFC");
  declareProperty("NtupleFile",   m_ntFile   = "FILE1");
  declareProperty("IsMC",         m_isMC=false);
  declareProperty("OFCTool",      m_OFCTool );
}

LArOFC2Ntuple::~LArOFC2Ntuple() 
{}

StatusCode LArOFC2Ntuple::initialize() {
  m_ntTitle="Optimal Filtering Coefficients";
  m_ntpath=std::string("/NTUPLES/")+m_ntFile+std::string("/")+m_ntName;

  if ( LArCond2NtupleBase::initialize().isFailure() ) {
	return StatusCode::FAILURE;
  }
  if ( m_isMC && m_OFCTool.retrieve().isFailure() ) {
        (*m_log) <<  MSG::ERROR << "OFC tool required for MC conditions but not available" << endmsg;
        return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArOFC2Ntuple::stop() {
  StatusCode sc;
  // Ntuple booking: Specific
  NTuple::Item<long> gain, phase, nSamples;
  NTuple::Item<float> timeOffset, phasetime;
  NTuple::Array<float> OFCa, OFCb;
  
  sc=m_nt->addItem("Gain",gain,-1,2);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'gain' failed" << endmsg;
    return StatusCode::FAILURE;
  }

  //Specific:
  sc=m_nt->addItem("TimeOffset",timeOffset,0,100);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'TimeOffset' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("Phase",phase,0,49);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'phase' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("PhaseTime",phasetime,0,800);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'PhaseTime' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("nSamples",nSamples,0,100);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'nSamples' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("OFCa",nSamples,OFCa);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'OFCa' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("OFCb",nSamples,OFCb);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'OFCb' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // retrieve OFC object 
  const ILArOFC* larOFC = NULL ;
  //const LArOFCComplete* larOFC = NULL ;
  if ( !m_isMC ) {
     (*m_log) <<  MSG::DEBUG << "Retrieving ILArOFC object with key " << m_contKey << endmsg;
     sc = m_detStore->retrieve(larOFC,m_contKey);
     if (sc.isFailure()) {
       (*m_log) <<  MSG::ERROR << "Can't retrieve ILArOFC with key " << m_contKey << " from Conditions Store" << endmsg;
       return StatusCode::FAILURE;
     }
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
  }

  unsigned cellCounter=0;  
  for ( unsigned igain=CaloGain::LARHIGHGAIN; 
	igain<CaloGain::LARNGAIN ; ++igain ) {
    std::vector<HWIdentifier>::const_iterator it = m_onlineId->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e = m_onlineId->channel_end();
    if ( m_isMC ) {
     for (;it!=it_e;it++) {
      const HWIdentifier chid = *it;
      if (!cabling->isOnlineConnected(chid)) continue;
        (*m_log) <<  MSG::VERBOSE << "Dumping OFC for channel 0x" << MSG::hex 
	  << chid.get_compact() << MSG::dec << endmsg;
        ILArOFC::OFCRef_t ofc_a=m_OFCTool->OFC_a(chid,igain);
        ILArOFC::OFCRef_t ofc_b=m_OFCTool->OFC_b(chid,igain);
	fillFromIdentifier(chid);
	gain  = (long)igain ;
	phase = (long)0 ;	  
	nSamples=ofc_a.size();
	for (int k=0;k<nSamples;k++ ) {
	  OFCa[k] = ofc_a[k] ;
	  OFCb[k] = ofc_b[k] ;
	}

	timeOffset = 0;	
	phasetime  = 0;
	
	sc = ntupleSvc()->writeRecord(m_nt);
	cellCounter++;
	if (sc!=StatusCode::SUCCESS) {
	  (*m_log) <<  MSG::ERROR << "writeRecord failed" << endmsg;
	  return StatusCode::FAILURE;
	}
     }//loop over channels
    }  else { // is_MC
     for (;it!=it_e;it++) {
      const HWIdentifier chid = *it;
      if ( !cabling->isOnlineConnected(chid)) continue;
      (*m_log) <<  MSG::VERBOSE << "Dumping OFC for channel 0x" << MSG::hex
          << chid.get_compact() << MSG::dec << endmsg;
      for (unsigned iphase=0;iphase<larOFC->nTimeBins(chid,igain);iphase++) {
        ILArOFC::OFCRef_t ofc_a=larOFC->OFC_a(chid,igain,iphase);
        //Check if we have OFC for this channel and gain
        //if (!ofc_a.size()) break;//No more phases
        ILArOFC::OFCRef_t ofc_b=larOFC->OFC_b(chid,igain,iphase);
        fillFromIdentifier(chid);
        gain  = (long)igain ;
        phase = (long)iphase ;
        nSamples=ofc_a.size();
        for (int k=0;k<nSamples;k++ ) {
          OFCa[k] = ofc_a[k] ;
          OFCb[k] = ofc_b[k] ;
        }

        timeOffset = larOFC->timeOffset(chid,igain);
        phasetime  = phase*larOFC->timeBinWidth(chid,igain);


        sc = ntupleSvc()->writeRecord(m_nt);
        cellCounter++;
        if (sc!=StatusCode::SUCCESS) {
          (*m_log) <<  MSG::ERROR << "writeRecord failed" << endmsg;
          return StatusCode::FAILURE;
        }
      }//loop over phases
     }//loop over channels
    } 
  }//loop over gains
  (*m_log) <<  MSG::INFO << "Total number of cells = " << cellCounter << endmsg;
  (*m_log) <<  MSG::INFO << "LArOFC2Ntuple has finished." << endmsg;
  return StatusCode::SUCCESS;
} // end finalize-method.
