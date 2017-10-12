/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRDO_Decoder.h"

using namespace Muon;

MdtRDO_Decoder::MdtRDO_Decoder(const std::string& type, const std::string& name,const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_mdtIdHelper(0),
  m_cablingSvc(0)
{  
  declareInterface< Muon::IMDT_RDO_Decoder  >( this );
}

StatusCode MdtRDO_Decoder::initialize() {
 
  if (StatusCode::SUCCESS != service("MuonMDT_CablingSvc", m_cablingSvc)) {
    ATH_MSG_ERROR(" Can't get MuonMDT_CablingSvc ");
    return StatusCode::FAILURE; 
  }  
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore", detStore);
  if (status.isFailure()) {
    ATH_MSG_FATAL("DetectorStore service not found !");
    return StatusCode::FAILURE;
  } else {}
  
  status = detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER");
  if (status.isFailure()) {
    ATH_MSG_FATAL("Could not get MdtIdHelper !");
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the MdtIdHelper. ");
  }


  return StatusCode::SUCCESS;
}

StatusCode MdtRDO_Decoder::finalize() 
{
  return StatusCode::SUCCESS;
}


//void MdtRDO_Decoder::set(const MdtIdHelper * mdtIdHelper, const MDTcablingSvc* mdtCabling) 
//{
//  m_mdtIdHelper = mdtIdHelper;
//  m_cablingSvc  = mdtCabling;
//}





