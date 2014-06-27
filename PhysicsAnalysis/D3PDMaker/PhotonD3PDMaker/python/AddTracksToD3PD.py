# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
import D3PDMakerCoreComps
import PhotonD3PDMaker
from D3PDMakerCoreComps.SimpleAssociation            import SimpleAssociation

from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject
from PhotonD3PDMaker.PhotonTrackD3PDObject import PhotonTrackParticleD3PDObject
from TrackD3PDMaker import TrackParticleVertexAssociationTool
from D3PDMakerCoreComps.resolveSGKey                 import testSGKey
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from D3PDMakerCoreComps.IndexAssociation             import IndexAssociation
from TrackD3PDMaker.TrackD3PDMakerFlags              import TrackD3PDFlags
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags

def AddTracksToD3PD(alg):
    
    # taken from TrackD3PDObject.py...  we only need to change the "vertexTarget/Prefix" stuff.
    TrackParticleD3PDObject = TrackD3PDObject(_label='trk',
                                              _prefix='trk_',
                                              _sgkey=D3PDMakerFlags.TrackSGKey(),
                                              _object_name='TrackParticleD3PDObject',
                                              typeName='Rec::TrackParticleContainer',
                                              vertexTarget='PV_',
                                              vertexPrefix='PV_',
                                              vertexSGKey='VxPrimaryCandidate',
                                              truthTarget='mc',
                                              truthPrefix='mc_',
                                              detailedTruthPrefix='detailed_mc_',
                                              truthMapKey='TrackParticleTruthCollection',
                                              SGKeyForTruth=D3PDMakerFlags.TrackSGKey(),
                                              detailedTruthMapKey='DetailedTrackTruth',
                                              SGKeyForDetailedTruth='Tracks',
                                              flags=TrackD3PDFlags)
    
    convTrackAssociation = SimpleAssociation \
                           (TrackParticleD3PDObject,
                            TrackParticleVertexAssociationTool,
                            prefix = 'conv_',
                            blockname='ConversionTrackParticle',
                            VxSGKey='ConversionCandidate')

    
    GSFTrackParticlesInSample = testSGKey ('Rec::TrackParticleContainer', "GSFTrackParticleCandidate")

    # we pretty much always want the GSF-refit tracks.
    if GSFTrackParticlesInSample:
        GSFTrackParticleD3PDObject = TrackD3PDObject(_label='trk',
	                                             _prefix='trk_',
	                                             _sgkey=D3PDMakerFlags.TrackSGKey(),
	                                             _object_name='GSFTrackParticleD3PDObject',
	                                             typeName='Rec::TrackParticleContainer',
	                                             vertexTarget='PV_',
	                                             vertexPrefix='PV_',
	                                             vertexSGKey='VxPrimaryCandidate',
	                                             truthTarget='mc',
	                                             truthPrefix='mc_',
	                                             detailedTruthPrefix='detailed_mc_',
	                                             truthMapKey='GSFTrackParticleTruthCollection',
	                                             SGKeyForTruth="GSFTrackParticleCandidate",
	                                             detailedTruthMapKey='DetailedTrackTruth',
	                                             SGKeyForDetailedTruth='Tracks',
	                                             flags=TrackD3PDFlags) 

        # add a flag for conversion tracks
        convTrackAssociation = SimpleAssociation \
                               (GSFTrackParticleD3PDObject,
                                TrackParticleVertexAssociationTool,
                                prefix = 'GSF_conv_',
                                blockname='ConversionTrackParticle',
                                VxSGKey='AllPhotonsVxCandidates')
        
        origTrackAssociation = IndexAssociation \
                               (GSFTrackParticleD3PDObject,
                                PhotonD3PDMaker.GSFOriginalTrackParticleAssociationTool,
                                target="trk",
                                prefix="trk_",
                                level=0,
                                blockname="GSFOriginalTPAssoc",
                                allowMissing=False,
                                AssocGetter = D3PDMakerCoreComps.SGObjGetterTool
                                ('GSFAssocGetter',
                                 SGKey = D3PDMakerFlags.GSFTrackAssocSGKey(),
                                 TypeName = 'TrackParticleAssocs'))

        alg += GSFTrackParticleD3PDObject (0,
                                           sgkey="GSFTrackParticleCandidate",
                                           label="GSF_trk",
                                           prefix="GSF_trk_")

    if PhotonD3PDMakerFlags.DumpAllTracks():
        alg += TrackParticleD3PDObject (4, sgkey="TrackParticleCandidate")
        
    elif PhotonD3PDMakerFlags.DumpNearbyTracks():
        from PhotonD3PDMaker.AddTrackFilter import AddTrackFilter
        preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())
        AddTrackFilter("SelectedTracks", preseq, GSFTrackParticlesInSample)
        
        alg += TrackParticleD3PDObject       (0, sgkey="SelectedTracks")
        alg += PhotonTrackParticleD3PDObject (0, sgkey="SelectedTracks")

    else:
        print "DumpAllTracks = False and DumpNearbyTracks = False...  not dumping any tracks."
