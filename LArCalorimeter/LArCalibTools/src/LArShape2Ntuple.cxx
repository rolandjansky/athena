/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArShape2Ntuple.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "LArRawConditions/LArShape32MC.h"
#include "LArCOOLConditions/LArShapeFlat.h"
#include "LArCOOLConditions/LArShapeSC.h"
#include "CaloIdentifier/CaloGain.h"

LArShape2Ntuple::LArShape2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator)
{
  declareProperty("ContainerKey", m_contKey  = "LArShape");
  //declareProperty("Nsamples",     m_nSamples = 5);
  //declareProperty("Nphases",      m_nPhases  = 50);
  declareProperty("NtupleName",   m_ntName   = "SHAPE");
  declareProperty("NtupleFile",   m_ntFile   = "FILE1");
  declareProperty("isMC",           m_isMC=false);
}

LArShape2Ntuple::~LArShape2Ntuple() 
{}


StatusCode LArShape2Ntuple::initialize() {
  m_ntTitle="Pulse Shape";
  m_ntpath=std::string("/NTUPLES/")+m_ntFile+std::string("/")+m_ntName;
  return LArCond2NtupleBase::initialize();
}


StatusCode LArShape2Ntuple::stop() {
  StatusCode sc;
  // Ntuple booking: Specific
  NTuple::Item<long> gain, phase, nSamples;
  NTuple::Item<float> timeOffset, phasetime;
  NTuple::Array<float> Shape, ShapeDer;

  //NTuple::Tuple *nt=m_nt;
  
  sc=m_nt->addItem("Gain",gain,-1,2);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'gain' failed" << endreq;
    return StatusCode::FAILURE;
  }

  //Specific:
  sc=m_nt->addItem("TimeOffset",timeOffset,0,100);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'TimeOffset' failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("Phase",phase,0,49);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'phase' failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("PhaseTime",phasetime,0,800);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'PhaseTime' failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("nSamples",nSamples,0,100);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'nSamples' failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("Shape",nSamples,Shape);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'Shape' failed" << endreq;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("ShapeDer",nSamples,ShapeDer);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) <<  MSG::ERROR << "addItem 'ShapeDer' failed" << endreq;
    return StatusCode::FAILURE;
  }
  
  // retrieve Shape object 
  //const ILArShape* IlarShape = NULL ;
  const LArShapeComplete* larShape = NULL ;
  const LArShape32MC* larShapeMC = NULL ;
  const LArShapeSC* larShapeSC = NULL ;
  const LArShapeFlat* larShapeFlat = NULL ;
  if ( m_isMC  && !m_isSC) {
  (*m_log) <<  MSG::DEBUG << "Retrieving LArShapeMC object" << endreq;
  sc = m_detStore->retrieve(larShapeMC,m_contKey);
  if (sc.isFailure()) {
    (*m_log) <<  MSG::ERROR << "Can't retrieve LArShapeMC from Conditions Store" << endreq;
    return StatusCode::FAILURE;
  }
  } else if (m_isSC) { 
  (*m_log) <<  MSG::DEBUG << "Retrieving LArShapeSC object" << endreq;
  sc = m_detStore->retrieve(larShapeSC,m_contKey);
  if (sc.isFailure()) {
    (*m_log) <<  MSG::ERROR << "Can't retrieve LArShapeSC from Conditions Store" << endreq;
    return StatusCode::FAILURE;
  }
  } else if (m_isFlat) { 
  (*m_log) <<  MSG::DEBUG << "Retrieving LArShapeFlat object" << endreq;
  sc = m_detStore->retrieve(larShapeFlat,m_contKey);
  if (sc.isFailure()) {
    (*m_log) <<  MSG::ERROR << "Can't retrieve LArShapeFlat from Conditions Store" << endreq;
    return StatusCode::FAILURE;
  }
  }else {
  (*m_log) <<  MSG::DEBUG << "Retrieving LArShape object" << endreq;
  sc = m_detStore->retrieve(larShape,m_contKey);
  if (sc.isFailure()) {
    (*m_log) <<  MSG::ERROR << "Can't retrieve LArShape from Conditions Store" << endreq;
    return StatusCode::FAILURE;
  }
  }

  (*m_log) <<  MSG::VERBOSE << " shape object retrieved" << endreq;
  unsigned cellCounter=0;  
  for ( unsigned igain=CaloGain::LARHIGHGAIN; 
	igain<CaloGain::LARNGAIN ; ++igain ) {
    std::vector<HWIdentifier>::const_iterator it = m_onlineId->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e = m_onlineId->channel_end();
    if ( m_isMC ) {
    for (;it!=it_e;it++) {
      const HWIdentifier chid = *it;
      if (!m_larCablingSvc->isOnlineConnected(chid)) continue;
      (*m_log) <<  MSG::VERBOSE << "Dumping ShapeMC for channel 0x" << MSG::hex 
	  << chid.get_compact() << MSG::dec << endreq;
        ILArShape::ShapeRef_t shape;
        if(m_isSC) shape=larShapeSC->Shape(chid,igain); else shape=larShapeMC->Shape(chid,igain);
        ILArShape::ShapeRef_t shapeder;
        if(m_isSC) shapeder=larShapeSC->ShapeDer(chid,igain); else shapeder=larShapeMC->ShapeDer(chid,igain);
	fillFromIdentifier(chid);
	gain  = (long)igain ;
	phase = (long)0 ;	  
	nSamples=shape.size();
	for (int k=0;k<nSamples;k++ ) {
	  Shape[k] = shape[k] ;
	  ShapeDer[k] = shapeder[k] ;
	}

	timeOffset = 0;	
	phasetime  = 0;

	
	sc = ntupleSvc()->writeRecord(m_nt);
	cellCounter++;
	if (sc!=StatusCode::SUCCESS) {
	  (*m_log) <<  MSG::ERROR << "writeRecord failed" << endreq;
	  return StatusCode::FAILURE;
	}
    }//loop over channels
    } else { // MC / not MC
    for (;it!=it_e;it++) {
      const HWIdentifier chid = *it;
      if (!m_larCablingSvc->isOnlineConnected(chid)) continue;
      (*m_log) <<  MSG::VERBOSE << "Dumping Shape for channel 0x" << MSG::hex
          << chid.get_compact() << MSG::dec << endreq;
      for (unsigned iphase=0;iphase<larShape->nTimeBins(chid,igain);iphase++) {
        ILArShape::ShapeRef_t shape;
        if(m_isSC) shape=larShapeSC->Shape(chid,igain,iphase);
        else if(m_isFlat) shape=larShapeFlat->Shape(chid,igain,iphase);
        else  shape=larShape->Shape(chid,igain,iphase);
        //Check if we have Shape for this channel and gain
        //if (!shape.size()) break;//No more phases
        ILArShape::ShapeRef_t shapeder;
        if(m_isSC) shapeder=larShapeSC->ShapeDer(chid,igain,iphase);
        else if(m_isFlat) shapeder=larShapeFlat->ShapeDer(chid,igain,iphase);
        else  shapeder=larShape->ShapeDer(chid,igain,iphase);
        fillFromIdentifier(chid);
        gain  = (long)igain ;
        phase = (long)iphase ;
        nSamples=shape.size();
        for (int k=0;k<nSamples;k++ ) {
          Shape[k] = shape[k] ;
          ShapeDer[k] = shapeder[k] ;
        }

        timeOffset = larShape->timeOffset(chid,igain);
        phasetime  = phase*larShape->timeBinWidth(chid,igain);


        sc = ntupleSvc()->writeRecord(m_nt);
        cellCounter++;
        if (sc!=StatusCode::SUCCESS) {
          (*m_log) <<  MSG::ERROR << "writeRecord failed" << endreq;
          return StatusCode::FAILURE;
        }
      }//loop over phases
    }//loop over channels
    }
  }//loop over gains
  (*m_log) <<  MSG::INFO << "Total number of cells = " << cellCounter << endreq;
  (*m_log) <<  MSG::INFO << "LArShape2Ntuple has finished." << endreq;
  return StatusCode::SUCCESS;
} // end finalize-method.
