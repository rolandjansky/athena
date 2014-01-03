# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: CombinedMuonFeatureD3PDObject.py 498371 2012-04-29 22:28:21Z ssnyder $
#
# D3PD object saving information about the LVL2 CombinedMuonFeature
# objects. Note that by default the object will try to associate ID tracks
# and MuonFeature objects both through an index matching and also through
# a "simple association". The user should usually disable one of these
# at least with something like:
#
# <code>
#   d3pdalg += CombinedMuonFeatureD3PDObject( 2, exclude = [ "MFIndex", "IDScanIndex",
#                                                            "SiTrkIndex" ] )
# </code>
#
# for instance.
#

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from RecExConfig.ObjKeyStore import cfgKeyStore
import EventCommonD3PDMaker
import TrigMuonD3PDMaker
import TrigInDetD3PDMaker

CombinedMuonFeatureD3PDObject = make_SGDataVector_D3PDObject( "CombinedMuonFeatureContainer",
                                                              "HLT",
                                                              "trig_L2_combmuonfeature_",
                                                              "CombinedMuonFeatureD3PDObject" )

#
# Basic CombinedMuonFeature information:
#
CombinedMuonFeatureD3PDObject.defineBlock( 0, "4Momentum",
                                           EventCommonD3PDMaker.FourMomFillerTool,
                                           WriteM = False )
CombinedMuonFeatureD3PDObject.defineBlock( 1, "CombInfo",
                                           TrigMuonD3PDMaker.CombinedMuonFeatureFillerTool )
# Chain decision information:
CombinedMuonFeatureD3PDObject.defineBlock( 1, 'Decision',
                                           TrigMuonD3PDMaker.CombinedMuonFeatureObjectDecisionFillerTool,
                                           AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                                           Chains = [ D3PDMakerFlags.MuonL2TrigPattern() ])

#
# "Associate" MuonFeature objects to the CombinedMuonFeature objects, and
# save the properties of these MF objects belonging to the CMF ones:
#
MFAssoc = SimpleAssociation( CombinedMuonFeatureD3PDObject,
                             TrigMuonD3PDMaker.CombinedMuonFeatureMuonFeatureAssociationTool,
                             matched = "mf",
                             blockname = "MFInfo",
                             prefix = "mf_" )
MFAssoc.defineBlock( 1, "MF4Momentum",
                     TrigMuonD3PDMaker.MuonFeature4MomFillerTool )

# Set up index association for the MuonFeature objects:
MFIndexAssoc = IndexAssociation( CombinedMuonFeatureD3PDObject,
                                 TrigMuonD3PDMaker.CombinedMuonFeatureMuonFeatureAssociationTool,
                                 target = "trig_L2_muonfeature_", prefix = "mf_", level = 1,
                                 blockname = "MFIndex", allowMissing = True )

#
# "Associate" TrigInDetTrack objects to the CombinedMuonFeature objects:
#
CombIDAssoc = SimpleAssociation( CombinedMuonFeatureD3PDObject,
                                 TrigMuonD3PDMaker.CombinedMuonFeatureIDTrackAssociationTool,
                                 matched = "hasidtrk",
                                 blockname = "IDInfo",
                                 prefix = "idtrk_" )
CombIDAssoc.defineBlock( 0, 'BasicInfo',
                         TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                         WriteBasicInfo = True )
CombIDAssoc.defineBlock( 1, 'HypoInfo',
                         TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                         WriteHypoInfo = True )
CombIDAssoc.defineBlock( 2, 'DetailedInfo',
                         TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                         WriteDetailedInfo = True )

CombIDFitParAssoc = SimpleAssociation( CombIDAssoc,
                                       TrigInDetD3PDMaker.TrigInDetTrackFitParAssociationTool,
                                       matched = "hasidtrkfitpar",
                                       blockname = "IDFitParInfo",
                                       prefix = "idtrkfitpar_" )
CombIDFitParAssoc.defineBlock( 0, "FitParBasicInfo",
                               TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                               WriteBasicInfo = True )
CombIDFitParAssoc.defineBlock( 2, "FitParDetailedInfo",
                               TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                               WriteDetailedInfo = True )

# Set up index association for the TrigInDetTrack objects:
IDScanIndexAssoc = IndexAssociation( CombinedMuonFeatureD3PDObject,
                                     TrigMuonD3PDMaker.CombinedMuonFeatureIDTrackAssociationTool,
                                     target = "trig_L2_idscan_muon_", prefix = "idscantrk_", level = 0,
                                     blockname = "IDScanIndex", allowMissing = True )
SiTrkIndexAssoc = IndexAssociation( CombinedMuonFeatureD3PDObject,
                                    TrigMuonD3PDMaker.CombinedMuonFeatureIDTrackAssociationTool,
                                    target = "trig_L2_sitrack_muon_", prefix = "sitracktrk_", level = 0,
                                    blockname = "SiTrkIndex", allowMissing = True )
if cfgKeyStore.isInInputFile( "TrigInDetTrackCollection",
                              "HLT_TrigL2SiTrackFinder_Muon" ):
    L2StarIndexAssoc = IndexAssociation( CombinedMuonFeatureD3PDObject,
                                         TrigMuonD3PDMaker.CombinedMuonFeatureIDTrackAssociationTool,
                                         target = "trig_L2_sitrackfinder_muon_",
                                         prefix = "sitrackfindertrk_", level = 0,
                                         blockname = "L2StarIndex", allowMissing = True )
if cfgKeyStore.isInInputFile( "TrigInDetTrackCollection",
                              "HLT_TrigL2SiTrackFinder" ):
    L2StarOldIndexAssoc = IndexAssociation( CombinedMuonFeatureD3PDObject,
                                            TrigMuonD3PDMaker.CombinedMuonFeatureIDTrackAssociationTool,
                                            target = "trig_L2_sitrackfinder_",
                                            prefix = "sitrackfinderoldtrk_", level = 0,
                                            blockname = "L2StarOldIndex", allowMissing = True )
