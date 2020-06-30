/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/MuonRdoToMuonDigit.h"

MuonRdoToMuonDigit::MuonRdoToMuonDigit(const std::string& name, 
				 ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("MuonRdoToMuonDigitTool", this )
{
   declareProperty("MuonRdoToMuonDigitTool", m_digTool);
}

StatusCode MuonRdoToMuonDigit::initialize()
{
  ATH_CHECK( m_digTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );
  return m_digTool->digitize(Gaudi::Hive::currentContext());
}

