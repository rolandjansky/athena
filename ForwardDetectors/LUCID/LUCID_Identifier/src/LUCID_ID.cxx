/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "LUCID_Identifier/LUCID_ID.h"
#include "LUCID_Identifier/LUCID_DetElemHash.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"  
#include <set>
#include <algorithm>
#include <iostream>
#include  <cassert>


//______________________________________________________
LUCID_ID::LUCID_ID(void) {

} 
//______________________________________________________
LUCID_ID::~LUCID_ID(void){

}
