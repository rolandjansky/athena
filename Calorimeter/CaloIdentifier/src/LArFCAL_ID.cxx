/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloIdentifier/LArFCAL_ID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"
#include "LArFCAL_region.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/MsgStream.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#define MAX_BUFFER_LEN 1024

LArFCAL_ID::LArFCAL_ID(void) : 
  LArFCAL_Base_ID("LArFCAL_ID", 0)
{
}

LArFCAL_ID::~LArFCAL_ID(void) 
{
}

int  LArFCAL_ID::initialize_from_dictionary (const IdDictMgr& dict_mgr)
/*=================================================================*/
{
  MsgStream log(m_msgSvc, "LArFCAL_ID" );

  std::string strg =  "initialize_from_dictionary";
  log << MSG::DEBUG << strg << endmsg;

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
						       "lar_fcal"))
  {
    if(dictionaryVersion() == "H8TestBeam" )
      return 0;
    return (1);
  }

  return 0;

}

