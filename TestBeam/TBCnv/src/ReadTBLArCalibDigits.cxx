/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCnv/ReadTBLArCalibDigits.h"
#include "LArIdentifier/LArOnlineID.h"
#include "GaudiKernel/AlgFactory.h"
#include <vector>

ReadTBLArCalibDigits::ReadTBLArCalibDigits(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_count(0),
    m_onlineHelper(0)
{
  declareProperty("ContainerKey",m_containerKey="");
  declareProperty("DumpFile",m_dumpFile="");
}

ReadTBLArCalibDigits::~ReadTBLArCalibDigits() 
{
}

StatusCode ReadTBLArCalibDigits::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize" << endreq;
  
  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper" << endreq;
    return StatusCode::FAILURE;
  }
  
  if (m_dumpFile.size()>0)
    m_outfile.open(m_dumpFile.c_str(),std::ios::out);

  m_count=0;
  
  log << MSG::INFO << "======== ReadTBLArCalibDigits initialize successfully ========" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode ReadTBLArCalibDigits::execute() {
  MsgStream log(msgSvc(), name());
  
  m_count++;

  int cellCounter = 0;
  
  StatusCode sc;
  
  log << MSG::DEBUG << "======== executing event "<< m_count << " ========" << endreq;
  log << MSG::DEBUG << "Retrieving TBLArCalibDigitContainer. Key= " << m_containerKey << endreq; 
  TBLArCalibDigitContainer* larCalibDigitCont;
  sc = evtStore()->retrieve(larCalibDigitCont ,m_containerKey);

  if (sc.isFailure()) {
    log << MSG::FATAL << " Cannot read TBLArCalibDigitContainer from StoreGate! key=" << m_containerKey << endreq;
    return StatusCode::FAILURE;
  }
  
  TBLArCalibDigitContainer::const_iterator it=larCalibDigitCont->begin();
  TBLArCalibDigitContainer::const_iterator it_end=larCalibDigitCont->end();
  
  log << MSG::VERBOSE << "Now loop over container " << endreq;
  
  for (;it!=it_end;it++) {

    log << MSG::VERBOSE << " Get hardware ID " << endreq;
    
    HWIdentifier chid=(*it)->hardwareID();
    
    log << MSG::VERBOSE << "  chid =  " << chid << endreq;

    const std::vector<short>& vSamples=(*it)->samples();    
    int nSamples=vSamples.size();
    
    log << MSG::VERBOSE << "  nSamples =  " << nSamples << endreq;

    if (m_outfile.is_open()) {
      m_outfile << "Evt="<< m_count << " ";
      m_outfile << "FebId= 0x" << std::hex << m_onlineHelper->feb_Id(chid).get_compact()
                << std::dec << " Ch= " << m_onlineHelper->channel(chid) << " ";
      for(int i=0; i<nSamples; i++) {
        m_outfile << " " << vSamples[i];
      }
      m_outfile << " G=" << (*it)->gain() << std::endl;
    }

    cellCounter++;

    log << MSG::VERBOSE << "  cellCounter =  " << cellCounter << endreq;
    
  }
  
  log << MSG::DEBUG  << "Event " << m_count << " contains " << cellCounter << " channels" << endreq;
  
  return StatusCode::SUCCESS;
  
}

StatusCode ReadTBLArCalibDigits::finalize()
{
  MsgStream log(msgSvc(), name());
  if (m_outfile.is_open()) 
    m_outfile.close();
  log << MSG::INFO << "finalize ReadTBLArCalibDigits" << endreq;
  return StatusCode::SUCCESS;
}
