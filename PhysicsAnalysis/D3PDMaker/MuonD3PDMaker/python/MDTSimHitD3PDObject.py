# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: MDTSimHitD3PDObject.py 508173 2012-06-29 11:47:55Z ssnyder $

# Import all needed modules:
import MuonD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject  import D3PDObject

# Create the configurable:
MDTSimHitD3PDObject = \
           make_SGDataVector_D3PDObject ('AtlasHitsVector<MDTSimHit>',
                                         'MDT_Hits',
                                         'mdt_hit_', 'MDTSimHitD3PDObject')

# Add blocks to it:
MDTSimHitD3PDObject.defineBlock( 0, "BasicInfo",
                                 MuonD3PDMaker.MDTSimHitFillerTool )
