/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MDT_IDDetDescrCnv.h"
#include "CSC_IDDetDescrCnv.h"
#include "RPC_IDDetDescrCnv.h"
#include "TGC_IDDetDescrCnv.h"
// for nSW
#include "sTGC_IDDetDescrCnv.h"
#include "MM_IDDetDescrCnv.h"
 
DECLARE_CONVERTER_FACTORY( MDT_IDDetDescrCnv )
DECLARE_CONVERTER_FACTORY( CSC_IDDetDescrCnv )
DECLARE_CONVERTER_FACTORY( RPC_IDDetDescrCnv )
DECLARE_CONVERTER_FACTORY( TGC_IDDetDescrCnv )

// for nSW
DECLARE_CONVERTER_FACTORY( sTGC_IDDetDescrCnv )
DECLARE_CONVERTER_FACTORY( MM_IDDetDescrCnv )

DECLARE_FACTORY_ENTRIES	( MuonIdCnv ) {
    DECLARE_CONVERTER	( MDT_IDDetDescrCnv );
    DECLARE_CONVERTER	( CSC_IDDetDescrCnv );
    DECLARE_CONVERTER	( RPC_IDDetDescrCnv );
    DECLARE_CONVERTER	( TGC_IDDetDescrCnv );
// for nSW
    DECLARE_CONVERTER	( sTGC_IDDetDescrCnv );
    DECLARE_CONVERTER	( MM_IDDetDescrCnv );
}



