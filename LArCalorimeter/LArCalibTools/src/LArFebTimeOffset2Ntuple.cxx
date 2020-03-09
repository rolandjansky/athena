/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArFebTimeOffset2Ntuple.h"
#include "LArRawConditions/LArFEBTimeOffset.h"

#include "LArIdentifier/LArOnlineID.h"

LArFebTimeOffset2Ntuple::LArFebTimeOffset2Ntuple(const std::string & name, ISvcLocator * pSvcLocator) :
  LArCond2NtupleBase(name,pSvcLocator) {
  
  declareProperty("Gain",m_gain=0);
  declareProperty("ContainerKey",m_inputContainer="FebTimeOffset");

  m_ntTitle="FebTimeOffsets";
  m_ntpath="/NTUPLES/FILE1/FEBTIME";
  return;
}

LArFebTimeOffset2Ntuple::~LArFebTimeOffset2Ntuple() {}


StatusCode LArFebTimeOffset2Ntuple::initialize() {

  //m_ntTitle="TimeDiffs";
  //m_ntpath=std::string("/NTUPLES/FILE1/TIMEDIFF");
  return LArCond2NtupleBase::initialize();
}

StatusCode LArFebTimeOffset2Ntuple::stop() {

  //std::cout << detStore()->dump() << std::endl;
  NTuple::Item<float> offset;
  NTuple::Item<long> gain;


  m_nt->addItem("gain",gain,0,3).ignore();
  m_nt->addItem("offset",offset,-5000,5000).ignore();

  const LArFEBTimeOffset* feboffsets;
  //if (detStore()->retrieve(feboffsets,m_inputContainer).isFailure()) {
  if (detStore()->retrieve(feboffsets).isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve LArFEBTimeOffset with key " << m_inputContainer );
    return StatusCode::FAILURE;
  }
  
  const float novalue=-99999;
  (const_cast<LArFEBTimeOffset*>(feboffsets))->setDefaultReturnValue(novalue);


  std::vector<HWIdentifier>::const_iterator fit=m_onlineId->feb_begin();
  std::vector<HWIdentifier>::const_iterator fit_e=m_onlineId->feb_end();
  for(;fit!=fit_e;++fit) {
    const HWIdentifier fid=*fit;
    offset=feboffsets->TimeOffset(fid);
    
    //FIXME: The PhysWave shifter fills 0 for all not-present FEBs.
    if (offset!=0 && offset!=novalue) {
      gain=m_gain;
      fillFromIdentifier(fid);

      StatusCode sc=ntupleSvc()->writeRecord(m_nt);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "writeRecord failed" );
	return StatusCode::FAILURE;
      }
    }//end if have value
  } // end loop over feb identifiers
  
  return StatusCode::SUCCESS;
}
