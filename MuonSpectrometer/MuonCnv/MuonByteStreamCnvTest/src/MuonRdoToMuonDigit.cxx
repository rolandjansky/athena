/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/MuonRdoToMuonDigit.h"
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

MuonRdoToMuonDigit::MuonRdoToMuonDigit(const std::string& name, 
				 ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_digTool("MuonRdoToMuonDigitTool", this )
{
   declareProperty("MuonRdoToMuonDigitTool", m_digTool);
}

MuonRdoToMuonDigit::~MuonRdoToMuonDigit()  {

}

StatusCode MuonRdoToMuonDigit::initialize()
{
  ATH_CHECK( m_digTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );
  return m_digTool->digitize();
}


StatusCode MuonRdoToMuonDigit::finalize()
{
  ATH_MSG_DEBUG( "finalize."  );
  return StatusCode::SUCCESS;
}


