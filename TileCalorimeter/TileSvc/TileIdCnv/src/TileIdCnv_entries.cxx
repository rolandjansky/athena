/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TileIdCnv/TileHWIDDetDescrCnv.h"

DECLARE_CONVERTER_FACTORY(TileHWIDDetDescrCnv)

DECLARE_FACTORY_ENTRIES	( TileIdCnv ) {
    DECLARE_CONVERTER	( TileHWIDDetDescrCnv );
}

