/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDOTOPREPDATA_IDC_HELPER_H
#define MUONRDOTOPREPDATA_IDC_HELPER_H

// Base classes
#include "Identifier/Identifier.h"
#include "Identifier/Identifiable.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "CxxUtils/checker_macros.h"

namespace Muon{

namespace IDC_Helper {
 
template< class IDC, class IDHELPER >
//DRDR static should match implementation in IDC_Helper.icc (clang32)
static  
typename IDC::IDENTIFIABLE* 
getCollection ATLAS_NOT_THREAD_SAFE (  const Identifier collId, 
                                       IDC* idc, 
                                       const IDHELPER& idHelper, 
                                       MsgStream& log);

template< class IDC, class IDHELPER >
typename IDC::IDENTIFIABLE* 
getCollection ATLAS_NOT_THREAD_SAFE (  const Identifier collId, 
                                       IDC* idc, 
                                       const IDHELPER* idHelper, 
                                       MsgStream& log)
{
  return getCollection<IDC, IDHELPER>(collId, idc, *idHelper, log);
}


template< class IDC, class IDHELPER >
//DRDR static should match implementation in IDC_Helper.icc (clang32)
typename IDC::IDENTIFIABLE* 
addCollection(  const Identifier collId, 
                IDC* idc, 
                const IDHELPER& idHelper, 
                MsgStream& log);


}
}

#include "IDC_Helper.icc"

#endif

