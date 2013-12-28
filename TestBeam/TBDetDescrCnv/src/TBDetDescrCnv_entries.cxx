/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TBDetDescrCnv/TBDetDescrCnv.h"

DECLARE_CONVERTER_FACTORY(TBDetDescrCnv)
DECLARE_FACTORY_ENTRIES	( TBDetDescrCnv ) {
    DECLARE_CONVERTER	( TBDetDescrCnv );
}

