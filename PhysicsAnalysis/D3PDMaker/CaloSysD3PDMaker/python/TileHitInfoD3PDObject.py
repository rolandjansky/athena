# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$

import CaloSysD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject

TileHitInfoD3PDObject = make_SG_D3PDObject( "AtlasHitsVector<TileHit>", "TileHitVec", "Tile_",
                                            "TileHitInfoInfoD3PDObject" )

TileHitInfoD3PDObject.defineBlock( 0, 'TileHitInfo',
                                   CaloSysD3PDMaker.TileHitInfoFillerTool,
                                   TimeMin = -12.5, TimeMax = 12.5, TimeOut = 99990. )


