# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: MuonFeatureD3PDObject.py 466851 2011-11-07 23:52:14Z ssnyder $
#
# D3PD object saving information about the LVL2 MuonFeature objects.
#

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TrigMuonD3PDMaker

MuonFeatureD3PDObject = make_SGDataVector_D3PDObject( "MuonFeatureContainer",
                                                      "HLT",
                                                      "trig_L2_muonfeature_",
                                                      "MuonFeatureD3PDObject" )

MuonFeatureD3PDObject.defineBlock( 0, "4Momentum",
                                   TrigMuonD3PDMaker.MuonFeature4MomFillerTool )
MuonFeatureD3PDObject.defineBlock( 1, "Reco",
                                   TrigMuonD3PDMaker.MuonFeatureFillerTool,
                                   WriteRecoInfo = True )
MuonFeatureD3PDObject.defineBlock( 2, "Hits",
                                   TrigMuonD3PDMaker.MuonFeatureFillerTool,
                                   WriteHitsInfo = True )

MuonFeatureD3PDObject.defineBlock( 1, 'Decision',
                                   TrigMuonD3PDMaker.MuonFeatureObjectDecisionFillerTool,
                                   AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                                   Chains = [ D3PDMakerFlags.MuonL2TrigPattern() ] )
