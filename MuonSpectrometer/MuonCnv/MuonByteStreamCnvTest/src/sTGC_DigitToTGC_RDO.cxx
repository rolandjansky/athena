/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "sTGC_DigitToTGC_RDO.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

sTGC_DigitToTGC_RDO::sTGC_DigitToTGC_RDO(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_idHelper(0)
{
}

StatusCode sTGC_DigitToTGC_RDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_rdoContainer.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode sTGC_DigitToTGC_RDO::execute()
{  
  ATH_MSG_DEBUG( "in execute()"  );

  ATH_MSG_DEBUG( "done execute()"  );
  return StatusCode::SUCCESS;
}

StatusCode sTGC_DigitToTGC_RDO::finalize() {
  return StatusCode::SUCCESS;
}
