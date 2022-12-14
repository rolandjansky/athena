/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/CheckLArFebHeader.h"
#include <vector>
#include "GaudiKernel/IToolSvc.h"
#include "LArRawEvent/LArFebHeaderContainer.h"

CheckLArFebHeader::CheckLArFebHeader(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_onlineHelper(0)
{
  m_count=0;
  //declareProperty("KeyList",m_keylistproperty);
}

CheckLArFebHeader::~CheckLArFebHeader() 
{
}

StatusCode CheckLArFebHeader::initialize()
{ 
  ATH_MSG_INFO ( "Initialize" );
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  m_count=0;  
  ATH_MSG_DEBUG ( "======== CheckLArFebHeader initialize successfully ========" );
  return StatusCode::SUCCESS;
}


StatusCode CheckLArFebHeader::execute()
{
  m_count++; 
  const LArFebHeaderContainer *larFebHeaderContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(larFebHeaderContainer) );

  for (const LArFebHeader* feb : *larFebHeaderContainer) {
    if(feb->CheckErrorELVL1Id() || feb->CheckErrorBCId()) {
      HWIdentifier febid=feb->FEBId();
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int FT        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);
      int BCID      = feb->BCId();
      int LVL1ID    = feb->ELVL1Id();
      int FebBCID   = feb->FebBCId();
      int FebLVL1ID = feb->FebELVL1Id();
	
      ATH_MSG_FATAL ( "TTC information mismatch in event " << m_count << ":" );
      ATH_MSG_FATAL ( "    FEBID = " << febid.get_compact() );
      ATH_MSG_FATAL ( "      BARREL/EC = " << barrel_ec << " POS/NEG = " << pos_neg );
      ATH_MSG_FATAL ( "      FT = " << FT << " SLOT = " << slot );
      ATH_MSG_FATAL ( "    TTC from ROD: LVL1ID = " << LVL1ID << " BCID = " << BCID );
      ATH_MSG_FATAL ( "    TTC from FEB: LVL1ID = " << FebLVL1ID << " BCID = " << FebBCID );
	
      return StatusCode::FAILURE;
    } // End if 
    else
      ATH_MSG_DEBUG ( "FEB header consistent." );
  } // End FebHeader loop

 return StatusCode::SUCCESS;
}

StatusCode CheckLArFebHeader::finalize()
{ 
  ATH_MSG_DEBUG ( ">>> Finalize" );
  return StatusCode::SUCCESS;
}
