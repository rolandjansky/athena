/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PixelIDDetDescrCnv.h"
#include "SCT_IDDetDescrCnv.h"
#include "SiliconIDDetDescrCnv.h"
#include "TRT_IDDetDescrCnv.h"

DECLARE_CONVERTER_FACTORY(SCT_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(PixelIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(SiliconIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(TRT_IDDetDescrCnv)

DECLARE_FACTORY_ENTRIES	( InDetIdCnv ) {
    DECLARE_CONVERTER	( PixelIDDetDescrCnv  );
    DECLARE_CONVERTER	( SCT_IDDetDescrCnv   );
    DECLARE_CONVERTER	( SiliconIDDetDescrCnv  );
    DECLARE_CONVERTER	( TRT_IDDetDescrCnv   );
}
