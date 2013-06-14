/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "IdDictDetDescrCnv.h"
#include "AtlasIDDetDescrCnv.h"
#include "IdDictCnvTest.h"

DECLARE_CONVERTER_FACTORY( AtlasIDDetDescrCnv )
DECLARE_CONVERTER_FACTORY( IdDictDetDescrCnv )
DECLARE_ALGORITHM_FACTORY( IdDictCnvTest )

DECLARE_FACTORY_ENTRIES	( IdDictDetDescrCnv ) {
    DECLARE_CONVERTER	( IdDictDetDescrCnv )
    DECLARE_CONVERTER	( AtlasIDDetDescrCnv )
    DECLARE_ALGORITHM	( IdDictCnvTest   )
}


