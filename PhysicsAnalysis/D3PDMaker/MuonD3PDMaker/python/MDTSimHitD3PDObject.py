# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Import all needed modules:
import MuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject

# Create the configurable:
MDTSimHitD3PDObject = \
           make_SGDataVector_D3PDObject ('AtlasHitsVector<MDTSimHit>',
                                         'MDT_Hits',
                                         'mdt_hit_', 'MDTSimHitD3PDObject')

# Add blocks to it:
MDTSimHitD3PDObject.defineBlock( 0, "BasicInfo",
                                 MuonD3PDMaker.MDTSimHitFillerTool )
