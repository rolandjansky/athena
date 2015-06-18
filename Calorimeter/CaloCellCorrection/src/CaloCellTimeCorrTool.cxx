/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellCorrection/CaloCellTimeCorrTool.h" 
#include "CaloEvent/CaloCell.h"
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"


CaloCellTimeCorrTool::CaloCellTimeCorrTool (const std::string& type, 
						const std::string& name, 
						const IInterface* parent) :
  CaloCellCorrection(type, name, parent),
  m_corrValues(0) { 
  declareInterface<CaloCellCorrection>(this); 
  declareProperty("Folder",m_key="/CALO/Ofl/CellTimeCorr",
		  "Key (=foldername) of AttrListCollection");
}
                        

CaloCellTimeCorrTool::~CaloCellTimeCorrTool() {}


StatusCode CaloCellTimeCorrTool::initialize() {
  StatusCode sc=detStore()->regFcn(&CaloCellTimeCorrTool::load,
				   dynamic_cast<CaloCellTimeCorrTool*>(this),
				   m_attrList,m_key);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Cound not register Callback function for AthenaAttributeList with key " << m_key << endreq;
     return sc;
  }
  return StatusCode::SUCCESS;
}


StatusCode CaloCellTimeCorrTool::load(IOVSVC_CALLBACK_ARGS) {

  ATH_MSG_INFO("in IOV callback function");
  if (m_corrValues) {
    ATH_MSG_INFO("Delete previously loaded correction values");
    delete m_corrValues;
    m_corrValues=0;
  }


  const coral::Blob& blob = (*m_attrList)["CaloCondBlob16M"].data<coral::Blob>();
  if (blob.size()<3) {
    ATH_MSG_INFO("Found empty blob, no corretion needed :-)");
  }
  else {
    m_corrValues=CaloCondBlobFlt::getInstance(blob);
    ATH_MSG_INFO("Database folder has vaues for " << m_corrValues->getNChans() << " channels and " << m_corrValues->getNGains() << " gains.");
  }
  return StatusCode::SUCCESS;
}


void CaloCellTimeCorrTool::MakeCorrection(CaloCell* theCell) {
  if (m_corrValues) {
    const IdentifierHash& hash_id=theCell->caloDDE()->calo_hash();
    if (hash_id<m_corrValues->getNChans()) {
      const float& shift= m_corrValues->getData(hash_id,0,0);
      theCell->setTime(theCell->time()+shift);
    }
  }
  return;
}
