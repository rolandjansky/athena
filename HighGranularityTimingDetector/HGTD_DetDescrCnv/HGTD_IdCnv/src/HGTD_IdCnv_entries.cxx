/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HGTD_IDDetDescrCnv.h"

DECLARE_CONVERTER_FACTORY(HGTD_IDDetDescrCnv)

DECLARE_FACTORY_ENTRIES ( HGTD_IdCnv ) {
    DECLARE_CONVERTER   ( HGTD_IDDetDescrCnv  );
}
