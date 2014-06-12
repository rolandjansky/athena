/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTest/DumpCaloBadChannels.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include <iostream>
#include <fstream>

DumpCaloBadChannels::DumpCaloBadChannels(const std::string & name, 
				   ISvcLocator * pSvcLocator) : 
  Algorithm(name,pSvcLocator),
  m_caloBCT("CaloBadChanTool"),
  m_cellID(0)

{
  declareProperty("FileName",m_fileName="");
}


DumpCaloBadChannels::~DumpCaloBadChannels() {}

                            
StatusCode DumpCaloBadChannels::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in Intialize()" << endreq;
  StatusCode sc;
  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     log << MSG::ERROR << "Unable to get the DetectorStore" << endreq;
     return sc;
   }
  sc = detStore->retrieve(m_cellID,"CaloCell_ID");    
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve CaloCell_ID helper from DetectorStore" << endreq;
    return sc;
  }
  sc=m_caloBCT.retrieve();
   if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve CaloBadChanTool" << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}




StatusCode  DumpCaloBadChannels::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  std::ostream *out = &(std::cout); 
  std::ofstream outfile;
  if (m_fileName.size()) {
    outfile.open(m_fileName.c_str(),std::ios::out);
    if (outfile.is_open()) {
      log << MSG::INFO << "Writing to file " << m_fileName << endreq;
      out = &outfile;
    }
    else
      log << MSG::ERROR << "Failed to open file " << m_fileName << endreq;
  }

  std::vector<Identifier>::const_iterator it=m_cellID->cell_begin();
  std::vector<Identifier>::const_iterator it_e=m_cellID->cell_end();
  
  for (;it!=it_e;it++) {
    const Identifier& id=*it;
    CaloBadChannel bc=m_caloBCT ->caloStatus(id); 
    (*out) << "Chan: 0x" << std::hex << id.get_compact() << ": " << bc.packedData() << std::dec << std::endl;
  }


  if (outfile.is_open())
      outfile.close();
  return StatusCode::SUCCESS;
}
StatusCode  DumpCaloBadChannels::execute() { 
  return StatusCode::SUCCESS;
}


