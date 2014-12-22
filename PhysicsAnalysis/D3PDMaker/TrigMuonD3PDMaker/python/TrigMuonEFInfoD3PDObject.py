# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigMuonEFInfoD3PDObject.py 637271 2014-12-22 02:43:34Z ssnyder $
#
# D3PD object saving information about the MuonEF & MuGirl
#

# from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
# from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
# from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
import EventCommonD3PDMaker
# import TrigMuonD3PDMaker

TrigMuonEFInfoD3PDObject = make_SGDataVector_D3PDObject( "DataVector<xAOD::Muon_v1>",
                                                         "HLT_MuonEFInfo",
                                                         "trig_EF_trigmuonef_",
                                                         "TrigMuonEFInfoD3PDObject" )
TrigMuonEFInfoD3PDObject.defineBlock (0, 'Kinematics',
                            EventCommonD3PDMaker.FourMomFillerTool,
                            WriteRect = True,
                            WriteE = True,
                            WriteM = True)

# # Chain decision information:
# TrigMuonEFInfoD3PDObject.defineBlock( 1, "Decision",
#                                       TrigMuonD3PDMaker.TrigMuonEFInfoObjectDecisionFillerTool,
#                                       AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
#                                       Chains = [ D3PDMakerFlags.MuonEFTrigPattern() ] )

# # Object source information:
# TrigMuonEFInfoD3PDObject.defineBlock( 1, "Source",
#                                       TrigMuonD3PDMaker.TrigMuonEFInfoObjectSourceFillerTool,
#                                       Sources = [ "EF_mu15.*[0-9]?mu[0-9]*_EFFS", "effs15",
#                                                   "EF_mu18.*[0-9]?mu[0-9]*_EFFS", "effs18",
#                                                   "EF_mu24.*[0-9]?mu[0-9]*_EFFS", "effs24" ] )

# TrigMuonEFInfoD3PDObject.defineBlock( 2, "MuonEF",
#                                       TrigMuonD3PDMaker.TrigMuonEFInfoFillerTool )

# #
# # Associate TrigMuonEFInfoTrack objects to the TrigMuonEFInfo objects:
# #
# InfoTrackAssoc = \
#    ContainedVectorMultiAssociation( TrigMuonEFInfoD3PDObject,
#                                     TrigMuonD3PDMaker.TrigMuonEFInfoTrackAssociationTool,
#                                     prefix = "track_", blockname = "InfoTrackAssoc" )

# InfoTrackAssoc.defineBlock( 0, "TrackDetails",
#                             TrigMuonD3PDMaker.TrigMuonEFInfoTrackFillerTool )

# #
# # Associate MS-only TrigMuonEFTrack objects to the TrigMuonEFInfoTrack objects:
# #
# SpectrometerTrackAssoc = \
#    SimpleAssociation( InfoTrackAssoc,
#                       TrigMuonD3PDMaker.TrigMuonEFInfoTrackSpectrometerTrackAssociationTool,
#                       matched = "hasMS", blockname = "SpectrometerTrackAssoc", prefix = "MS_" )

# SpectrometerTrackAssoc.defineBlock( 0, "SpectrometerTrackBasics",
#                                     EventCommonD3PDMaker.FourMomFillerTool,
#                                     WriteM = False )
# SpectrometerTrackAssoc.defineBlock( 1, "SpectrometerTracFitResults",
#                                     TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                     SaveFitResults = True)
# SpectrometerTrackAssoc.defineBlock( 2, "SpectrometerTrackDetails",
#                                     TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                     SaveHitInfo = True )

# #
# # Associate extrapolated TrigMuonEFTrack objects to the TrigMuonEFInfoTrack objects:
# #
# ExtrapolatedTrackAssoc = \
#    SimpleAssociation( InfoTrackAssoc,
#                       TrigMuonD3PDMaker.TrigMuonEFInfoTrackExtrapolatedTrackAssociationTool,
#                       matched = "hasSA", blockname = "ExtrapolatedTrackAssoc", prefix = "SA_" )

# ExtrapolatedTrackAssoc.defineBlock( 0, "ExtrapolatedTrackBasics",
#                                     EventCommonD3PDMaker.FourMomFillerTool,
#                                     WriteM = False )
# ExtrapolatedTrackAssoc.defineBlock( 1, "ExtrapolatedTrackFitResults",
#                                     TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                     SaveFitResults = True )

# #
# # Associate combined TrigMuonEFCbTrack objects to the TrigMuonEFInfoTrack objects:
# #
# CombinedTrackAssoc = \
#    SimpleAssociation( InfoTrackAssoc,
#                       TrigMuonD3PDMaker.TrigMuonEFInfoTrackCombinedTrackAssociationTool,
#                       matched = "hasCB", blockname = "CombinedTrackAssoc", prefix = "CB_" )

# CombinedTrackAssoc.defineBlock( 0, "CombinedTrackBasics",
#                                 EventCommonD3PDMaker.FourMomFillerTool,
#                                 WriteM = False )
# CombinedTrackAssoc.defineBlock( 1, "CombinedTrackFitResults1",
#                                 TrigMuonD3PDMaker.TrigMuonEFTrackFillerTool,
#                                 SaveFitResults = True )
# CombinedTrackAssoc.defineBlock( 1, "CombinedTrackFitResults2",
#                                 TrigMuonD3PDMaker.TrigMuonEFCbTrackFillerTool,
#                                 SaveFitResults = True )
# CombinedTrackAssoc.defineBlock( 2, "CombinedTrackFitDetails",
#                                 TrigMuonD3PDMaker.TrigMuonEFCbTrackFillerTool,
#                                 SaveHitInfo = True )

