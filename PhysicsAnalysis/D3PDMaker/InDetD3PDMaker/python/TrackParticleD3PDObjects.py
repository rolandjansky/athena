# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from InDetD3PDMaker.TrackD3PDObject import TrackD3PDObject
import TrackD3PDMaker
import InDetD3PDMaker

class TrackParticleType:
    typeName = 'Rec::TrackParticleContainer'
    PerigeeAssociationTool       = TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool
    PerigeeAtPVAssociationTool   = TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool
    PerigeeAtBSAssociationTool   = TrackD3PDMaker.TrackParticlePerigeeAtBSAssociationTool
    FitQualityAssociationTool    = InDetD3PDMaker.TrackParticleFitQualityAssociationTool
    TrackSummaryAssociationTool  = InDetD3PDMaker.TrackParticleTrackSummaryAssociationTool
    TrackInfoAssociationTool     = TrackD3PDMaker.TrackParticleInfoAssociationTool
    HitAssociationTool           = InDetD3PDMaker.TrackParticleTSOAssociationTool
    VertexAssociationTool        = TrackD3PDMaker.TrackParticleVertexAssociationTool
    TruthAssociationTool         = InDetD3PDMaker.TrackParticleTruthAssociationTool
    DetailedTruthAssociationTool = InDetD3PDMaker.TrackParticleDetailedTruthAssociationTool
    

TrackParticleD3PDObject = TrackD3PDObject(_label='trk',
                                          _prefix='trk_',
                                          _sgkey=D3PDMakerFlags.TrackSGKey(),
                                          _object_name='TrackParticleD3PDObject',
                                          typ=TrackParticleType,
                                          vertexTarget='vx',
                                          vertexPrefix='vx_',
                                          vertexSGKey='VxPrimaryCandidate',
                                          truthTarget='mc',
                                          truthPrefix='mc_',
                                          detailedTruthPrefix='detailed_mc_',
                                          truthMapKey='TrackParticleTruthCollection',
                                          SGKeyForTruth=D3PDMakerFlags.TrackSGKey(),
                                          detailedTruthMapKey='DetailedTrackTruth',
                                          SGKeyForDetailedTruth='Tracks',
                                          flags=TrackD3PDFlags)

