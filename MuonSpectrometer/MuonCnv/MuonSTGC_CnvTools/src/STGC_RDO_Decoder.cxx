/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "STGC_RDO_Decoder.h"

using namespace Muon;

STGC_RDO_Decoder::STGC_RDO_Decoder(const std::string& type, const std::string& name,const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_stgcIdHelper(nullptr)
{  
  declareInterface< Muon::ISTGC_RDO_Decoder  >( this );
}

StatusCode STGC_RDO_Decoder::initialize() {

  ATH_CHECK(detStore()->retrieve(m_stgcIdHelper, "STGCIDHELPER"));

  return StatusCode::SUCCESS;
}
