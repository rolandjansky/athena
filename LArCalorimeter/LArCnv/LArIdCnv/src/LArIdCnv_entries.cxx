/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

//<doc><file>	$Id: LArIdCnv_entries.cxx,v 1.6 2007-03-05 09:15:09 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LArIdCnv/LArIdMgrDetDescrCnv.h"
#include "LArIdCnv/LArOnlineIDDetDescrCnv.h"
#include "LArIdCnv/LArOnline_SuperCellIDDetDescrCnv.h"
#include "LArIdCnv/LArHVLineIDDetDescrCnv.h"
#include "LArIdCnv/LArElectrodeIDDetDescrCnv.h"

DECLARE_CONVERTER_FACTORY(LArElectrodeIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArHVLineIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArIdMgrDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArOnlineIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArOnline_SuperCellIDDetDescrCnv)

DECLARE_FACTORY_ENTRIES	( LArIdCnv ) {
    DECLARE_CONVERTER	( LArIdMgrDetDescrCnv );
    DECLARE_CONVERTER	( LArOnlineIDDetDescrCnv );
    DECLARE_CONVERTER	( LArHVLineIDDetDescrCnv );
    DECLARE_CONVERTER	( LArElectrodeIDDetDescrCnv );
    DECLARE_CONVERTER	( LArOnline_SuperCellIDDetDescrCnv );
}
