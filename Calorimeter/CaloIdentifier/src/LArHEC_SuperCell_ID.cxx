/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/LArHEC_SuperCell_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Helper class for LArHEC offline identifiers for supercells
 */


#include "CaloIdentifier/LArHEC_SuperCell_ID.h"


LArHEC_SuperCell_ID::LArHEC_SuperCell_ID()
  : LArHEC_Base_ID ("LArHEC_SuperCell_ID", true)
{
}


int  LArHEC_SuperCell_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "LArHEC_SuperCell_ID" );
  log << MSG::DEBUG << "initialize_from_dictionary" << endmsg;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    if(m_msgSvc)log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  // init base object
  if (LArHEC_Base_ID::initialize_base_from_dictionary(dict_mgr, "slar_hec"))
    return (1);

  return 0;
}

