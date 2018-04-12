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
  declareProperty("isComplete",   m_isComplete=false);
  
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
    ATH_MSG_ERROR( "addItem 'gain' failed" );
    return StatusCode::FAILURE;
  }

  //Specific:
  if (m_isComplete) {
    sc=m_nt->addItem("TimeOffset",timeOffset,0,100);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'TimeOffset' failed" );
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("Phase",phase,0,49);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'phase' failed" );
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("PhaseTime",phasetime,0,800);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'PhaseTime' failed" );
      return StatusCode::FAILURE;
    }
  }
  sc=m_nt->addItem("nSamples",nSamples,0,100);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'nSamples' failed" );
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("Shape",nSamples,Shape);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Shape' failed" );
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("ShapeDer",nSamples,ShapeDer);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'ShapeDer' failed" );
    return StatusCode::FAILURE;
  }
  
  const ILArShape* larShape = NULL ;
  const LArShapeComplete* larShapeComplete = NULL ;

  if (m_isComplete) {
    sc = detStore()->retrieve(larShapeComplete,m_contKey);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Can't retrieve LArShapeComplete object with key " << m_contKey );
      return StatusCode::FAILURE;
    }
    larShape=larShapeComplete; //Cast to base-class
  }
  else { //Use just the abstract interface (works also for LArShapeFlat and LArShapeMC)
    sc = detStore()->retrieve(larShape,m_contKey);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Can't retrieve ILArShape object with key " << m_contKey );
      return StatusCode::FAILURE;
    }
  }

  unsigned cellCounter=0;  
  for ( unsigned igain=CaloGain::LARHIGHGAIN; 
	igain<CaloGain::LARNGAIN ; ++igain ) {
    std::vector<HWIdentifier>::const_iterator it = m_onlineId->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e = m_onlineId->channel_end();
    for (;it!=it_e;it++) {
      const HWIdentifier chid = *it;
      if (!m_larCablingSvc->isOnlineConnected(chid)) continue;
      unsigned nPhase=1;
      if (larShapeComplete) nPhase=larShapeComplete->nTimeBins(chid,gain);
      for (unsigned iphase=0;iphase<nPhase;iphase++) {
	ATH_MSG_VERBOSE("Dumping Shape for channel " << m_onlineId->channel_name(chid) << ", gain " << gain << ", phase " << iphase);
	ILArShape::ShapeRef_t shape=larShape->Shape(chid,igain);
        ILArShape::ShapeRef_t shapeder =larShape->ShapeDer(chid,igain);
	fillFromIdentifier(chid);
	gain  = (long)igain ;
	nSamples=shape.size();
	for (int k=0;k<nSamples;k++ ) {
	  Shape[k] = shape[k] ;
	  ShapeDer[k] = shapeder[k] ;
	}
	if (larShapeComplete) {
	  timeOffset = larShapeComplete->timeOffset(chid,igain);
	  phasetime  = phase*larShapeComplete->timeBinWidth(chid,igain);
	  phase = (long)iphase ;
	}

	sc = ntupleSvc()->writeRecord(m_nt);
	cellCounter++;
	if (sc!=StatusCode::SUCCESS) {
	  ATH_MSG_ERROR( "writeRecord failed" );
	  return StatusCode::FAILURE;
	}
      }//loop over phases
    }//loop over channels
  }//loop over gains
     
  ATH_MSG_INFO( "Total number of cells = " << cellCounter );
  ATH_MSG_INFO( "LArShape2Ntuple has finished." );
  return StatusCode::SUCCESS;
} // end finalize-method.
