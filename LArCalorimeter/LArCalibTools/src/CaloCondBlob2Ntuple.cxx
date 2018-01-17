/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/CaloCondBlob2Ntuple.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

CaloCondBlob2Ntuple::CaloCondBlob2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) { 
  declareProperty("Folder",m_folder);

  m_ntTitle="CaloCondBlob";
  m_ntpath="/NTUPLES/FILE1/CaloCondBlob";
   

}

CaloCondBlob2Ntuple::~CaloCondBlob2Ntuple() 
{}

StatusCode CaloCondBlob2Ntuple::stop () {
  StatusCode sc;

//   NTuple::Array<float> values;
//   NTuple::Item<long> nValues;

//   sc=m_nt->addItem("nvalues",nValues,0,1000);
//   if (sc!=StatusCode::SUCCESS)
//     {(*m_log)  << MSG::ERROR << "addItem 'nvalues' failed" << endmsg;
//     return StatusCode::FAILURE;
//     }
 
//   sc=m_nt->addItem("values",nValues,values);
//   if (sc!=StatusCode::SUCCESS)
//     {(*m_log)  << MSG::ERROR << "addItem 'values' failed" << endmsg;
//     return StatusCode::FAILURE;
//     }


  NTuple::Item<float> value;
  sc=m_nt->addItem("value",value,0,100);
  if (sc!=StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "addItem 'values' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  
  const AthenaAttributeList* attrList=0;
  sc=detStore()->retrieve(attrList,m_folder);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve AthenaAttributeList with key " << m_folder << endmsg;
    return sc;
  }

  const coral::Blob& blob = (*attrList)["CaloCondBlob16M"].data<coral::Blob>();
  if (blob.size()<3) {
    ATH_MSG_INFO("Found empty blob, nothing to do");
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<const CaloCondBlobFlt> condBlob (CaloCondBlobFlt::getInstance(blob));
  ATH_MSG_INFO("Database folder has values for " << condBlob->getNChans() << " channels and " << condBlob->getNGains() << " gains.");
  

  const CaloCell_ID* caloCellId;
  sc = m_detStore->retrieve(caloCellId,"CaloCell_ID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get CaloCellID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  
  const unsigned nChans=condBlob->getNChans();
  //const usnigend nGains=condBlob->getNGains();
    

  for(unsigned h=0;h<nChans;++h) {
    //add here loop over gains
     //const Identifier id=caloCellId->cell_id(h);
     const HWIdentifier hwid=m_larCablingSvc->createSignalChannelIDFromHash(h);   
     fillFromIdentifier(hwid);
     value= condBlob->getData(h,0,0);
     
     sc=ntupleSvc()->writeRecord(m_nt);
     if (sc!=StatusCode::SUCCESS) {
       (*m_log)  << MSG::ERROR << "writeRecord failed" << endmsg;
       return StatusCode::FAILURE;
     }
  }//end loop over hash ids

 (*m_log)  << MSG::INFO << "CaloCondBlob2Ntuple has finished." << endmsg;
 return StatusCode::SUCCESS;
}// end finalize-method.
