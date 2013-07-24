/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "LArCalibUtils/LArHVCorrMaker.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArIdentifier/LArOnlineID.h"

#include <math.h>
#include <unistd.h>


LArHVCorrMaker::LArHVCorrMaker(const std::string& name, ISvcLocator* pSvcLocator) 
  : Algorithm(name, pSvcLocator),
    m_detStore(0),
    m_lar_on_id(0),
    m_hvCorrTool("LArHVCorrTool")
{
}

//---------------------------------------------------------------------------
LArHVCorrMaker::~LArHVCorrMaker()
{}

//---------------------------------------------------------------------------
StatusCode LArHVCorrMaker::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "  in initialize " << endreq;

  StatusCode  sc = m_hvCorrTool.retrieve();
  if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to find tool for LArHVCorrTool" << endreq; 
      return StatusCode::FAILURE;
  }

  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
      log << MSG::ERROR
          << "Unable to retrieve pointer to DetectorStore "
          << endreq;
      return sc;
  }


  sc = m_detStore->retrieve(m_lar_on_id,"LArOnlineID");
  if (sc.isFailure()) {
    log  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore"
         << endreq;
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArHVCorrMaker::execute()
{
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArHVCorrMaker::stop()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " in stop" << endreq;

  StatusCode sc = m_hvCorrTool->record();
  if (sc.isFailure()) {
     log << MSG::ERROR << " failure to record HVScaleCorr in detector store" << endreq;
     return sc;
  }

  const ILArHVScaleCorr* m_scale=NULL;
  sc = m_detStore->retrieve(m_scale,"LArHVScaleCorr");
  if (sc.isFailure() || !m_scale) {
     log << MSG::ERROR << " failure to read back LArHVScaleCorr " << endreq;
     return sc;
  }

 // get HWIdentifier iterator
  std::vector<HWIdentifier>::const_iterator it  = m_lar_on_id->channel_begin();
  std::vector<HWIdentifier>::const_iterator it_e= m_lar_on_id->channel_end();

  // loop over Identifiers
  for(;it!=it_e;++it)
  {    
    const HWIdentifier id  = *it;  
    float scale = m_scale->HVScaleCorr(id);
    log << MSG::DEBUG <<  m_lar_on_id->show_to_string(id) << " " << scale << endreq;
  }
    


  return StatusCode::SUCCESS;
}
