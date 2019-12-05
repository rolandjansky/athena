/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 
// Local include(s):
#include "xAODMissingETContainerCnv.h"

xAOD::MissingETContainer*
xAODMissingETContainerCnv::createTransientWithKey (const std::string& key)
{
  xAOD::MissingETContainer* c = xAODMissingETContainerCnvBase::createTransientWithKey (key);
  for(xAOD::MissingET* metObj : *c) metObj->updateHash();
  return c;
}
