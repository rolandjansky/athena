# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
import D3PDMakerCoreComps
import PhotonD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags

DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool

def AddTrackFilter(outputsgkey = "SelectedTracks",
                   seq = AlgSequence(),
                   GSFTrackParticlesInSample = False):

    from AthenaCommon.AppMgr      import ToolSvc

    # This is a filter for tracks near photon candidates.  It will look in a region around
    # the photon, and dump the track information for all tracks that fall inside that region.
    print "DumpAllTracks = False and DumpNearbyTracks = True...  dumping nearby tracks."

    photongetter = DVGetter('photontrackfilter_Getter',
                            TypeName = 'PhotonContainer',
                            SGKey = D3PDMakerFlags.PhotonSGKey())
    ToolSvc += photongetter

    electrongetter = DVGetter('electrontrackfilter_Getter',
                              TypeName = 'ElectronContainer',
                              SGKey = D3PDMakerFlags.ElectronSGKey())
    ToolSvc += electrongetter

    gsfgetter = DVGetter('gsftracktrackfilter_Getter',
                         TypeName = 'Rec::TrackParticleContainer',
                         SGKey = "GSFTrackParticleCandidate")
    ToolSvc += gsfgetter
    
    muongetter = DVGetter('muontrackfilter_Getter',
                          TypeName = 'Analysis::MuonContainer',
                          SGKey = D3PDMakerFlags.MuonSGKey())
    ToolSvc += muongetter


    gsfassocgetter = D3PDMakerCoreComps.SGObjGetterTool('GSFTPAssocGetter',
                                                        SGKey = D3PDMakerFlags.GSFTrackAssocSGKey(),
                                                        TypeName = 'TrackParticleAssocs')
    ToolSvc += gsfassocgetter

    from ROOT import egammaPID
    trackfilter = PhotonD3PDMaker.TrackFilter(InputTrackCollectionName = "TrackParticleCandidate",
                                              OutputTrackCollectionName = "SelectedTracks",
                                              photonGetter = photongetter,
                                              electronGetter = electrongetter,
                                              CheckGSFTracks=GSFTrackParticlesInSample,
                                              GSFTrackParticleGetter = gsfgetter,
                                              GSFAssocGetter = gsfassocgetter,
                                              muonGetter = muongetter,
                                              ptCut = 12000.,
                                              etaCut = 2.5,
                                              deltaRCut = 0.6,
                                              photonIsEM = egammaPID.PhotonLoose,
                                              electronIsEM = egammaPID.ElectronTight,
                                              storePerpendicularTracks = True,
                                              FilterTracksForMET = PhotonD3PDMakerFlags.FilterTracksForMET())

    seq += trackfilter
    
