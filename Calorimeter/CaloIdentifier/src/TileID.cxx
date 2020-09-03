/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Access to Tile Calorimeter raw data
 -----------------------------------------
 ***************************************************************************/

//<doc><file>   $Id: TileID.cxx,v 1.23 2008-07-10 00:55:04 solodkov Exp $
//<version>     $Name: not supported by cvs2svn $


#include "CaloIdentifier/TileID.h"
#include "IdDict/IdDictDefs.h"
#include "Identifier/IdentifierHash.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GaudiKernel/MsgStream.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <stdexcept>


TileID::TileID( void ) 
  : Tile_Base_ID ("TileID", false)
{
}

int TileID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
{
  MsgStream log(m_msgSvc, "TileID" );
  
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
  if (Tile_Base_ID::initialize_base_from_dictionary(dict_mgr, "tile"))
    return (1);

  return 0;
}


