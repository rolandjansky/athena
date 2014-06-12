/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTest/LArFakeCellFactor.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloConditions/CaloCellFactor.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "StoreGate/StoreGateSvc.h"
#include <iostream>
#include <fstream>

LArFakeCellFactor::LArFakeCellFactor(const std::string & name, 
				   ISvcLocator * pSvcLocator) : 
  Algorithm(name,pSvcLocator) {
  declareProperty("CorrectionKey",m_key="FakeScale",
		  "Key of the CaloCellFactor object to produced");
  declareProperty("InputFileName",m_fileName="",
		  "Text file containing the correction");

}


LArFakeCellFactor::~LArFakeCellFactor() {}

                            
StatusCode LArFakeCellFactor::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " Building fake CellFactor object " << endreq;
  StatusCode sc;
  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     log << MSG::ERROR << "Unable to get the DetectorStore" << endreq;
     return sc;
   }
  const CaloCell_ID* cellID;
  sc = detStore->retrieve(cellID);    
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve caloCell_ID helper from DetectorStore" << endreq;
    return sc;
  }
  // Get hash range
  IdentifierHash emMin, emMax;//, hecMin, hecMax, fcalMin, fcalMax;
  cellID->calo_cell_hash_range(CaloCell_ID::LAREM,emMin,emMax);
  log << MSG::DEBUG << "Hash range LArEM: " << emMin << " - " << emMax 
      << endreq;
  
  //Build new CaloCellFactor object
  CaloRec::CaloCellFactor* newScale=new CaloRec::CaloCellFactor(emMax);
  sc=detStore->record(newScale,m_key);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to record object CaloCellFactor with key " 
	<< m_key << " to detector store." << endreq;
    delete newScale;
    return sc;
  }
  if (m_fileName.size()>0) { //Have input file
    //Helper values to interpret the file
    const float etaHalfSize=0.1;
    const float phiHalfSize=0.196349546; //pi/8
    struct FileEntry entry;
    std::vector<FileEntry> valueList;
    unsigned num; //Not really used....
    std::ifstream infile(m_fileName.c_str());
    if (!infile.good()) {
      log << MSG::ERROR<< "Could not open file " << m_fileName << endreq;
      return StatusCode::FAILURE;
    }
    while(infile.good()) {
      infile >> num >> entry.eta >> entry.phi >> entry.value;
      log << MSG::DEBUG << "Read: " << num << " "<< entry.eta << " "
	  << entry.phi << " " << entry.value << endreq;
      valueList.push_back(entry);
    }
    log << MSG::DEBUG << "Read " << valueList.size() << " factors from file " 
	<< m_fileName << endreq;
    //Get DetDescrManager:
    const CaloDetDescrManager* caloDetDescrMan = CaloDetDescrManager::instance();
    unsigned filled=0;
    unsigned notFilled=0;
    //Now fill CaloCellFactor object
    for (unsigned i=emMin;i<emMax;i++) {
      //const CaloIdentifier cellId=cell_id(i);
      const CaloDetDescrElement* caloDDE=caloDetDescrMan->get_element(i);
      float etaCell=caloDDE->eta();
      float phiCell=caloDDE->phi();
      //Search eta/phi in list of values
      //In principle this could be done much faster if we assume that the list
      //is ordered.
      std::vector<FileEntry>::const_iterator it=valueList.begin();
      std::vector<FileEntry>::const_iterator it_e=valueList.end();
      for(;it!=it_e;++it) {
	if (etaCell<it->eta+etaHalfSize && etaCell>it->eta-etaHalfSize && 
	    phiCell<it->phi+phiHalfSize && phiCell>it->phi-phiHalfSize) {
	  if ((*newScale)[i] != 1.0) 
	    log << MSG::WARNING << " Value for eta=" << etaCell  
		<< ", phi= " << phiCell << " already filled (" << 
		(*newScale)[i] <<  ")!" << endreq;
	  (*newScale)[i]=it->value+1.0;
	  filled++;
	  log << MSG::DEBUG << "Found value " << it->value 
	      << " for eta=" << etaCell  << ", phi= " << phiCell 
	      << " found in file " << m_fileName << endreq;
	  break;
	}
      }// end loop searching for value in valueList
      if (it==it_e) {
	notFilled++;
	log << MSG::DEBUG << "No value for eta=" << etaCell 
	    << ", phi= " << phiCell << " found in file " << m_fileName 
	    << endreq;
      }
    }//End loop over all cells.
    log << MSG::INFO << "Found values for " << filled << " cells. " 
	<< notFilled << " cells not found, used default value" << endreq; 
    //Lock object here?
  }//end if fileName.size()>0
  return StatusCode::SUCCESS;
}


StatusCode  LArFakeCellFactor::execute() 
{ 
  return StatusCode::SUCCESS;
}

StatusCode  LArFakeCellFactor::finalize() 
{ 
  return StatusCode::SUCCESS;
}
