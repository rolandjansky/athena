# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TileTrackMuFeatureD3PDObject.py 466851 2011-11-07 23:52:14Z ssnyder $
#
# D3PD object saving information about the LVL2 TileTrackMuFeature
# objects. Note that by default the object will try to associate TileMuFeature
# objects both through an index matching and also through "simple association".
# The user should disable one of these at least with something like:
#
# <code>
#   d3pdalg += TileTrackMuFeatureD3PDObject( 2, exclude = [ "TileMuIndex" ] )
# </code>
#
# for instance.
#

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
import EventCommonD3PDMaker
import TrigMuonD3PDMaker
import TrigInDetD3PDMaker


TileTrackMuFeatureD3PDObject = make_SGDataVector_D3PDObject( "TileTrackMuFeatureContainer",
                                                             "HLT",
                                                             "trig_L2_tiletrackmufeature_",
                                                             "TileTrackMuFeatureD3PDObject" )

#
# Basic TileTrackMuFeature information:
#
TileTrackMuFeatureD3PDObject.defineBlock( 0, "4Momentum",
                                          EventCommonD3PDMaker.FourMomFillerTool,
                                          WriteM = False )
TileTrackMuFeatureD3PDObject.defineBlock( 1, "TileTrackMuInfo",
                                          TrigMuonD3PDMaker.TileTrackMuFeatureFillerTool )

# Chain decision information:
TileTrackMuFeatureD3PDObject.defineBlock( 1, 'Decision',
                                          TrigMuonD3PDMaker.TileTrackMuFeatureObjectDecisionFillerTool,
                                          AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                                          Chains = [ D3PDMakerFlags.MuonL2TrigPattern() ] )

#
# "Associate" TileMuFeature objects to the TileTrackMuFeature objects, and
# save the properties:
#
TileAssoc = SimpleAssociation( TileTrackMuFeatureD3PDObject,
                               TrigMuonD3PDMaker.TileTrackMuFeatureTileMuFeatureAssociationTool,
                               matched = "hastilemu",
                               blockname = "TileMuInfo",
                               prefix = "tilemu_" )
TileAssoc.defineBlock( 1, "AssocTileMu",
                       TrigMuonD3PDMaker.TileMuFeatureFillerTool,
                       WriteAngle = True, WriteDetails = True )

# Set up index association for the TileMuFeature objects:
TileIndexAssoc = IndexAssociation( TileTrackMuFeatureD3PDObject,
                                   TrigMuonD3PDMaker.TileTrackMuFeatureTileMuFeatureAssociationTool,
                                   target = "trig_L2_tilemufeature_", prefix = "tilemu_", level = 0,
                                   blockname = "TileMuIndex", allowMissing = True )

#
# "Associate" TrigInDetTrack objects to the TileTrackMuFeature objects. The TrigInDetTrack
# collections for these objects are not saved by default. So the index matching is not
# configured.
#
TileIDAssoc = SimpleAssociation( TileTrackMuFeatureD3PDObject,
                                 TrigMuonD3PDMaker.TileTrackMuFeatureIDTrackAssociationTool,
                                 matched = "hasidtrk",
                                 blockname = "IDInfo",
                                 prefix = "idtrk_" )
TileIDAssoc.defineBlock( 0, 'BasicInfo',
                         TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                         WriteBasicInfo = True )
TileIDAssoc.defineBlock( 1, 'HypoInfo',
                         TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                         WriteHypoInfo = True )
TileIDAssoc.defineBlock( 2, 'DetailedInfo',
                         TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                         WriteDetailedInfo = True )

TileIDFitParAssoc = SimpleAssociation( TileIDAssoc,
                                       TrigInDetD3PDMaker.TrigInDetTrackFitParAssociationTool,
                                       matched = "hasidtrkfitpar",
                                       blockname = "IDFitParInfo",
                                       prefix = "idtrkfitpar_" )
TileIDFitParAssoc.defineBlock( 0, "FitParBasicInfo",
                               TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                               WriteBasicInfo = True )
TileIDFitParAssoc.defineBlock( 2, "FitParDetailedInfo",
                               TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                               WriteDetailedInfo = True )
