/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistencySvc/IPersistencySvc.h"
#include "PersistencySvc.h"

std::unique_ptr< pool::IPersistencySvc >
pool::IPersistencySvc::create( IFileCatalog& catalog )
{
   return std::unique_ptr<IPersistencySvc>( new PersistencySvc::PersistencySvc(catalog) );
}
