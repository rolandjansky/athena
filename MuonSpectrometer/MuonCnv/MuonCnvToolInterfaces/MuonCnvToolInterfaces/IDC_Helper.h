/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_IDC_HELPER_H
#define MUONRDOTOPREPDATA_IDC_HELPER_H

// Base classes
#include "Identifier/Identifier.h"
#include "Identifier/Identifiable.h"
#include "Identifier/IdentifierHash.h"
#include "DataModel/DataVector.h"

#include "CLIDSvc/CLASS_DEF.h"

namespace Muon{

namespace IDC_Helper {
 
template< class IDC, class IDHELPER >
//DRDR static should match implementation in IDC_Helper.icc (clang32)
static  
typename IDC::IDENTIFIABLE* 
getCollection(  const Identifier collId, 
                IDC* idc, 
                const IDHELPER* idHelper, 
                MsgStream& log);

}
}

#include "IDC_Helper.icc"

#endif

