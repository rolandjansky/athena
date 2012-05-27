# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigInDetTrackD3PDObject.py 340544 2011-01-18 12:51:04Z krasznaa $
#
# D3PD object saving information about the LVL2 inner detector tracks
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
import TrigInDetD3PDMaker

TrigInDetTrackContainer_sgkey = 'HLT_TrigIDSCAN_eGamma'
TrigInDetTrackD3PDObject = make_SGDataVector_D3PDObject( 'TrigInDetTrackCollection',
                                                         TrigInDetTrackContainer_sgkey,
                                                         'trig_L2_tracks_',
                                                         'TrigInDetTrackD3PDObject' )

#
# Information about the TrigInDetTrack itself:
#
TrigInDetTrackD3PDObject.defineBlock( 0, 'BasicInfo',
                                      TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                                      WriteBasicInfo = True )

TrigInDetTrackD3PDObject.defineBlock( 1, 'HypoInfo',
                                      TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                                      WriteHypoInfo = True )

TrigInDetTrackD3PDObject.defineBlock( 2, 'DetailedInfo',
                                      TrigInDetD3PDMaker.TrigInDetTrackFillerTool,
                                      WriteDetailedInfo = True )

#
# "Associate" TrigInDetTrackFitPar object (at the start of the track)
# to the TrigInDetTracks. Then, save the properties of the fit parameter object
# using the dedicated filler tool.
#
FitParAssoc = SimpleAssociation( TrigInDetTrackD3PDObject,
                                 TrigInDetD3PDMaker.TrigInDetTrackFitParAssociationTool,
                                 matched = "", blockname = "FitParameters",
                                 prefix = "param_" )
FitParAssoc.defineBlock( 0, "FitParBasicInfo",
                         TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                         WriteBasicInfo = True )
FitParAssoc.defineBlock( 2, "FitParDetailedInfo",
                         TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                         WriteDetailedInfo = True )

#
# "Associate" TrigInDetTrackFitPar object (at the start of the track)
# to the TrigInDetTracks. Then, save the properties of the fit parameter object
# using the dedicated filler tool.
#
EndFitParAssoc = SimpleAssociation( TrigInDetTrackD3PDObject,
                                    TrigInDetD3PDMaker.TrigInDetTrackEndFitParAssociationTool,
                                    matched = "", blockname = "EndFitParameters",
                                    prefix = "endParam_" )
EndFitParAssoc.defineBlock( 2, "EndFitParBasicInfo",
                            TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                            WriteBasicInfo = True )
EndFitParAssoc.defineBlock( 3, "EndFitParDetailedInfo",
                            TrigInDetD3PDMaker.TrigInDetTrackFitParFillerTool,
                            WriteDetailedInfo = True )
