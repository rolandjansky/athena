# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: IsoMuonFeatureD3PDObject.py 466851 2011-11-07 23:52:14Z ssnyder $
#
# D3PD object saving information about the LVL2 IsoMuonFeature
# objects.
#

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import TrigMuonD3PDMaker

IsoMuonFeatureD3PDObject = make_SGDataVector_D3PDObject( "IsoMuonFeatureContainer",
                                                         "HLT",
                                                         "trig_L2_isomuonfeature_",
                                                         "IsoMuonFeatureD3PDObject" )

#
# Basic IsoMuonFeature information:
#
IsoMuonFeatureD3PDObject.defineBlock( 0, "4Momentum",
                                      EventCommonD3PDMaker.FourMomFillerTool,
                                      WriteM = False )
IsoMuonFeatureD3PDObject.defineBlock( 0, "HypoInfo",
                                      TrigMuonD3PDMaker.IsoMuonFeatureFillerTool,
                                      WriteHypoInfo=True )
IsoMuonFeatureD3PDObject.defineBlock( 1, "DetailedInfo",
                                      TrigMuonD3PDMaker.IsoMuonFeatureFillerTool,
                                      WriteDetailedInfo=True )
# Chain decision information
IsoMuonFeatureD3PDObject.defineBlock( 1, 'Decision',
                                      TrigMuonD3PDMaker.IsoMuonFeatureObjectDecisionFillerTool,
                                      AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                                      Chains = [ D3PDMakerFlags.MuonL2TrigPattern() ] )
