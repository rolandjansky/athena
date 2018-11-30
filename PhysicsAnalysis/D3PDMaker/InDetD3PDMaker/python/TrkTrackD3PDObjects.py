# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from InDetD3PDMaker.TrackD3PDObject import TrackD3PDObject
import InDetD3PDMaker


class TrackCollectionType:
    typeName = 'TrackCollection'
    PerigeeAssociationTool       = InDetD3PDMaker.TrkTrackPerigeeAssociationTool
    PerigeeAtPVAssociationTool   = InDetD3PDMaker.TrkTrackPerigeeAtPVAssociationTool
    PerigeeAtBSAssociationTool   = InDetD3PDMaker.TrkTrackPerigeeAtBSAssociationTool
    FitQualityAssociationTool    = InDetD3PDMaker.TrkTrackFitQualityAssociationTool
    TrackSummaryAssociationTool  = InDetD3PDMaker.TrkTrackTrackSummaryAssociationTool
    TrackInfoAssociationTool     = InDetD3PDMaker.TrkTrackInfoAssociationTool
    HitAssociationTool           = InDetD3PDMaker.TrkTrackTSOAssociationTool
    VertexAssociationTool        = InDetD3PDMaker.TrkTrackVertexAssociationTool
    TruthAssociationTool         = InDetD3PDMaker.TrkTrackTruthAssociationTool
    DetailedTruthAssociationTool = InDetD3PDMaker.TrkTrackDetailedTruthAssociationTool


TrkTrackD3PDObject = TrackD3PDObject(_label='trkTrack',
                                     _prefix='trkTrack_',
                                     _sgkey='Tracks',
                                     _object_name='TrkTrackD3PDObject',
                                     typ=TrackCollectionType,
                                     vertexTarget='vx',
                                     vertexPrefix='vx_',
                                     vertexSGKey='VxPrimaryCandidate',
                                     truthTarget='mc',
                                     truthPrefix='mc_',
                                     detailedTruthPrefix='detailed_mc_',
                                     truthMapKey='TrackTruthCollection',
                                     SGKeyForTruth='Tracks',
                                     detailedTruthMapKey='DetailedTrackTruth',
                                     SGKeyForDetailedTruth='Tracks')

PixelTrackD3PDObject = TrackD3PDObject(_label='trkPix',
                                       _prefix='trkPix_',
                                       _sgkey='ResolvedPixelTracks',
                                       _object_name='PixelTrackD3PDObject',
                                       typ=TrackCollectionType,
                                       vertexTarget='vx',
                                       vertexPrefix='vx_',
                                       vertexSGKey='VxPrimaryCandidate',
                                       truthTarget='mc',
                                       truthPrefix='mc_',
                                       detailedTruthPrefix='detailed_mc_',
                                       truthMapKey='ResolvedPixelTracksTruthCollection',
                                       SGKeyForTruth='ResolvedPixelTracks',
                                       detailedTruthMapKey='ResolvedPixelTracksDetailedTruth',
                                       SGKeyForDetailedTruth='ResolvedPixelTracks')

SCTTrackD3PDObject = TrackD3PDObject(_label='trkSCT',
                                     _prefix='trkSCT_',
                                     _sgkey='ResolvedSCTTracks',
                                     _object_name='SCTTrackD3PDObject',
                                     typ=TrackCollectionType,
                                     vertexTarget='vx',
                                     vertexPrefix='vx_',
                                     vertexSGKey='VxPrimaryCandidate',
                                     truthTarget='mc',
                                     truthPrefix='mc_',
                                     detailedTruthPrefix='detailed_mc_',
                                     truthMapKey='ResolvedSCTTracksTruthCollection',
                                     SGKeyForTruth='ResolvedSCTTracks',
                                     detailedTruthMapKey='ResolvedSCTTracksDetailedTruth',
                                     SGKeyForDetailedTruth='ResolvedSCTTracks')

TRTTrackD3PDObject = TrackD3PDObject(_label='trkTRT',
                                     _prefix='trkTRT_',
                                     _sgkey='StandaloneTRTTracks',
                                     _object_name='TRTTrackD3PDObject',
                                     typ=TrackCollectionType,
                                     vertexTarget='vx',
                                     vertexPrefix='vx_',
                                     vertexSGKey='VxPrimaryCandidate',
                                     truthTarget='mc',
                                     truthPrefix='mc_',
                                     detailedTruthPrefix='detailed_mc_',
                                     truthMapKey='StandaloneTRTTracks',
                                     SGKeyForTruth='StandaloneTRTTracks',
                                     detailedTruthMapKey='StandaloneTRTTracksDetailedTruth',
                                     SGKeyForDetailedTruth='StandaloneTRTTracks')

ResolvedTracksD3PDObject = TrackD3PDObject(_label='trkResolvedTrack',
                                           _prefix='trkResolvedTrack_',
                                           _sgkey='ResolvedTracks',
                                           _object_name='ResolvedTracksD3PDObject',
                                           typ=TrackCollectionType,
                                           vertexTarget='vx',
                                           vertexPrefix='vx_',
                                           vertexSGKey='VxPrimaryCandidate',
                                           truthTarget='mc',
                                           truthPrefix='mc_',
                                           detailedTruthPrefix='detailed_mc_',
                                           truthMapKey='ResolvedTracksTruthCollection',
                                           SGKeyForTruth='ResolvedTracks',
                                           detailedTruthMapKey='ResolvedTracksDetailedTruth',
                                           SGKeyForDetailedTruth='ResolvedTracks')

