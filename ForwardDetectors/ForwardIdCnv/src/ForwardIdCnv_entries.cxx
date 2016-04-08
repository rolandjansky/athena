/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ZdcIDDetDescrCnv.h"

DECLARE_CONVERTER_FACTORY(ZdcIDDetDescrCnv)

DECLARE_FACTORY_ENTRIES	( ForwardIdCnv ) {
    DECLARE_CONVERTER	( ZdcIDDetDescrCnv  );
}
