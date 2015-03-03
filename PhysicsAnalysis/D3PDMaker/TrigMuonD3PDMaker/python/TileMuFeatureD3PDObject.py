# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TileMuFeatureD3PDObject.py 466851 2011-11-07 23:52:14Z ssnyder $
#
# D3PD object saving information about the LVL2 TileMuFeature
# objects.
#

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TrigMuonD3PDMaker

TileMuFeatureD3PDObject = make_SGDataVector_D3PDObject( "TileMuFeatureContainer",
                                                        "HLT",
                                                        "trig_L2_tilemufeature_",
                                                        "TileMuFeatureD3PDObject" )

#
# Basic TileMuFeature information:
#
TileMuFeatureD3PDObject.defineBlock( 0, "TileMuAngle",
                                     TrigMuonD3PDMaker.TileMuFeatureFillerTool,
                                     WriteAngle = True)
TileMuFeatureD3PDObject.defineBlock( 1, "TileMuDetails",
                                     TrigMuonD3PDMaker.TileMuFeatureFillerTool,
                                     WriteDetails = True)
# Chain decision information
TileMuFeatureD3PDObject.defineBlock( 1, 'Decision',
                                     TrigMuonD3PDMaker.TileMuFeatureObjectDecisionFillerTool,
                                     AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                                     Chains = [ D3PDMakerFlags.MuonL2TrigPattern() ] )
