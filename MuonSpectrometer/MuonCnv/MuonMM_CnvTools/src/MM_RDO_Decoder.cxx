/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MM_RDO_Decoder.h"

using namespace Muon;

MM_RDO_Decoder::MM_RDO_Decoder(const std::string& type, const std::string& name,const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_mmIdHelper(nullptr)
{  
  declareInterface< Muon::IMM_RDO_Decoder  >( this );
}

StatusCode MM_RDO_Decoder::initialize() {
   
  ATH_CHECK(detStore()->retrieve(m_mmIdHelper, "MMIDHELPER"));

  return StatusCode::SUCCESS;
}
