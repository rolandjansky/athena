/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/LArFCAL_SuperCell_ID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"

#include "GaudiKernel/MsgStream.h"

#include <cmath>
#include <iostream>
#include <set>
#include <string>

LArFCAL_SuperCell_ID::LArFCAL_SuperCell_ID(void) :
  LArFCAL_Base_ID("LArFCAL_SuperCell_ID", 1)
{
}

LArFCAL_SuperCell_ID::~LArFCAL_SuperCell_ID(void) 
{
}

int  LArFCAL_SuperCell_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "LArFCAL_SuperCell_ID" );

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
  if (LArFCAL_Base_ID::initialize_base_from_dictionary(dict_mgr,
						       "slar_fcal"))
    return (1);
  
  return 0;
}

