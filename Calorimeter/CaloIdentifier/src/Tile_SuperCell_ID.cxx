/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/src/Tile_SuperCell_ID.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2013
 * @brief Helper class for Tile offline identifiers for supercells
 */


#include "CaloIdentifier/Tile_SuperCell_ID.h"
#include "IdDict/IdDictDefs.h"
#include "Identifier/IdentifierHash.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <stdexcept>


Tile_SuperCell_ID::Tile_SuperCell_ID()
  : Tile_Base_ID ("Tile_SuperCell_ID", true)
{
}

int Tile_SuperCell_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
{
  MsgStream log(m_msgSvc, "Tile_SuperCell_ID" );
  
  std::string strg = "initialize_from_dictionary";
  log << MSG::DEBUG << strg << endmsg;

  // Check whether this helper should be reinitialized
  if (!reinitialize(dict_mgr)) {
    log << MSG::DEBUG << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
    return (0);
  }
  else {
    log << MSG::DEBUG << "(Re)initialize" << endmsg;
  }

  // init base object
  if (Tile_Base_ID::initialize_base_from_dictionary(dict_mgr, "tile_supercell"))
    return (1);

  return 0;
}
