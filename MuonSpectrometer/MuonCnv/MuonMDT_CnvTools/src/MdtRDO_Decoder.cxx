/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRDO_Decoder.h"

using namespace Muon;

MdtRDO_Decoder::MdtRDO_Decoder(const std::string& type, const std::string& name,const IInterface* parent) :
  AthAlgTool(type,name,parent)
{  
  declareInterface< Muon::IMDT_RDO_Decoder  >( this );
}

StatusCode MdtRDO_Decoder::initialize() {
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK( m_readKey.initialize() );
  return StatusCode::SUCCESS;
}
