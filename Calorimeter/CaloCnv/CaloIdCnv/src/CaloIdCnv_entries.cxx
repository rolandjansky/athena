/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================

//<doc><file>	$Id: CaloIdCnv_entries.cxx,v 1.4 2009-05-14 16:35:13 solodkov Exp $
//<version>	$Name: not supported by cvs2svn $


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloIdCnv/CaloIdMgrDetDescrCnv.h"
#include "CaloIdCnv/CaloCell_IDDetDescrCnv.h"
#include "CaloIdCnv/CaloCell_SuperCell_IDDetDescrCnv.h"
#include "CaloIdCnv/CaloDM_IDDetDescrCnv.h"
#include "CaloIdCnv/CaloLVL1_IDDetDescrCnv.h"
#include "CaloIdCnv/LArEM_IDDetDescrCnv.h"
#include "CaloIdCnv/LArEM_SuperCell_IDDetDescrCnv.h"
#include "CaloIdCnv/LArFCAL_IDDetDescrCnv.h"
#include "CaloIdCnv/LArFCAL_SuperCell_IDDetDescrCnv.h"
#include "CaloIdCnv/LArMiniFCAL_IDDetDescrCnv.h"
#include "CaloIdCnv/LArHEC_IDDetDescrCnv.h"
#include "CaloIdCnv/LArHEC_SuperCell_IDDetDescrCnv.h"
#include "CaloIdCnv/TileIDDetDescrCnv.h"
#include "CaloIdCnv/Tile_SuperCell_IDDetDescrCnv.h"
#include "CaloIdCnv/TileTBIDDetDescrCnv.h"
#include "CaloIdCnv/TTOnlineIDDetDescrCnv.h"
#include "CaloIdCnv/JTower_IDDetDescrCnv.h"
#include "CaloIdCnv/GTower_IDDetDescrCnv.h"

DECLARE_CONVERTER_FACTORY(CaloCell_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(CaloCell_SuperCell_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(CaloDM_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(CaloIdMgrDetDescrCnv)
DECLARE_CONVERTER_FACTORY(CaloLVL1_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArEM_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArEM_SuperCell_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArFCAL_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArFCAL_SuperCell_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArMiniFCAL_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArHEC_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(LArHEC_SuperCell_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(TileIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(Tile_SuperCell_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(TileTBIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(TTOnlineIDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(JTower_IDDetDescrCnv)
DECLARE_CONVERTER_FACTORY(GTower_IDDetDescrCnv)

DECLARE_FACTORY_ENTRIES	( CaloIdCnv ) {
    DECLARE_CONVERTER	( CaloIdMgrDetDescrCnv );
    DECLARE_CONVERTER	( CaloCell_IDDetDescrCnv );
    DECLARE_CONVERTER	( CaloCell_SuperCell_IDDetDescrCnv );
    DECLARE_CONVERTER	( CaloDM_IDDetDescrCnv );
    DECLARE_CONVERTER	( CaloLVL1_IDDetDescrCnv );
    DECLARE_CONVERTER	( LArEM_IDDetDescrCnv );
    DECLARE_CONVERTER	( LArEM_SuperCell_IDDetDescrCnv );
    DECLARE_CONVERTER	( LArFCAL_IDDetDescrCnv );
    DECLARE_CONVERTER	( LArFCAL_SuperCell_IDDetDescrCnv );
    DECLARE_CONVERTER	( LArMiniFCAL_IDDetDescrCnv );
    DECLARE_CONVERTER	( LArHEC_IDDetDescrCnv );
    DECLARE_CONVERTER	( LArHEC_SuperCell_IDDetDescrCnv );
    DECLARE_CONVERTER	( TileIDDetDescrCnv );
    DECLARE_CONVERTER	( Tile_SuperCell_IDDetDescrCnv );
    DECLARE_CONVERTER	( TileTBIDDetDescrCnv );
    DECLARE_CONVERTER	( TTOnlineIDDetDescrCnv );
    DECLARE_CONVERTER   ( JTower_IDDetDescrCnv );
    DECLARE_CONVERTER   ( GTower_IDDetDescrCnv );
}
