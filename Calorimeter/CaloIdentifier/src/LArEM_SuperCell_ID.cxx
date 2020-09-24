/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/LArEM_SuperCell_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Helper class for LArEM offline identifiers for supercells
 */


#include "CaloIdentifier/LArEM_SuperCell_ID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "LArEM_region.h"

#include "GaudiKernel/MsgStream.h"

#include <cmath>
#include <iostream>
#include <set>
#include <string>



LArEM_SuperCell_ID::LArEM_SuperCell_ID(void) :
  LArEM_Base_ID("LArEM_SuperCell_ID", 1)
{
}

LArEM_SuperCell_ID::~LArEM_SuperCell_ID(void) 
{
}

int  LArEM_SuperCell_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "LArEM_SuperCell_ID" );

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
  if (LArEM_Base_ID::initialize_base_from_dictionary(dict_mgr,
                                                     "slar_em"))
    return (1);

  return 0;
}

