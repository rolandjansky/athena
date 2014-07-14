# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file D3PDMakerConfig/python/makexAOD.py
# @author scott snyder <snyder@bnl.gov>
# @date Jun, 2014
# @brief Make xAOD objects from AOD if needed.
#
# ??? Shouldn't this be done in RecExCommon?
#


from AthenaCommon.AlgSequence                 import AlgSequence
from RecExConfig.ObjKeyStore                  import cfgKeyStore


def convert_clusters (seq, xaod_key, key):
    from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator
    alg = ClusterCreator (xaod_key + 'Cnv',
                          AODContainerNames=[key],
                          xAODContainerNames=[xaod_key])
    seq += alg
    return


def convert_jets (seq, xaod_key, key):
    constit_key = None
    constit_search = False
    if key.find ('Topo') >= 0:
        constit_key = 'CaloCalTopoCluster'
    elif key.find ('Truth') >= 0:
        constit_key = 'TruthParticle'
        constit_search = True
        
    from xAODJetCnv.xAODJetCnvConf import xAODMaker__JetCnvAlg
    alg = xAODMaker__JetCnvAlg (xaod_key + 'Cnv',
                                InputJetCollection = key,
                                xAODJetContainer = xaod_key,
                                ConstituentContainer = constit_key,
                                ConstitSearch = constit_search)
    seq += alg
    return


def convert_trackparticles (seq, xaod_key, key, truth_key,
                            trackmap_key = None,
                            base_xaod_key = None, base_key = None):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    have_truth = cfgKeyStore.isInInput ('TrackParticleTruthCollection',
                                        truth_key)
    alg = xAODMaker__TrackParticleCnvAlg \
          (xaod_key + 'Cnv',
           AODContainerName = key,
           xAODContainerName = xaod_key,
           AODTruthContainerName = truth_key,
           AddTruthLink = have_truth)
    seq += alg

    if trackmap_key != None:
        from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__GSFTrackParticleMapCnvAlg
        alg = xAODMaker__GSFTrackParticleMapCnvAlg\
              (xaod_key + 'ParticleMapCnv',
               AODContainerName = base_key,
               AODContainerNameGSF = key,
               xAODContainerName = base_xaod_key,
               xAODContainerNameGSF = xaod_key,
               TrackMapName = trackmap_key)
        seq += alg

    return


def convert_tracks (seq, xaod_key, key, truth_key):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    have_truth = cfgKeyStore.isInInput ('TrackTruthCollection',
                                        truth_key)
    alg = xAODMaker__TrackParticleCnvAlg \
          (xaod_key + 'Cnv',
           AddTruthLink = have_truth,
           TrackContainerName = key,
           xAODTrackParticlesFromTracksContainerName = xaod_key,
           ConvertTracks = True,
           ConvertTrackParticles = False)
    
    seq += alg
    return


def convert_vertices (seq, xaod_key, key, TPContainerName):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__VertexCnvAlg
    alg = xAODMaker__VertexCnvAlg (xaod_key + 'Cnv',
                                   AODContainerName = key,
                                   xAODContainerName = xaod_key,
                                   TPContainerName = TPContainerName)
    seq += alg
    return


def convert_electrons (seq, xaod_key, key, xAODContainerFrwdName):
    from AthenaCommon.AppMgr          import ToolSvc
    from egammaTools.EMPIDBuilderBase import EMPIDBuilderElectronBase
    pid = EMPIDBuilderElectronBase("electronPIDBuilder")
    ToolSvc += pid

    from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__ElectronCnvTool
    tool = xAODMaker__ElectronCnvTool (xaod_key + 'CnvTool',
                                       xAODElectronOrigTrackContainerName = 'TrackParticles',
                                       PIDBuilder = pid)
    ToolSvc += tool
    
    from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__ElectronCnvAlg
    alg = xAODMaker__ElectronCnvAlg (xaod_key + 'Cnv',
                                     CnvTool = tool,
                                     AODContainerName = key,
                                     xAODContainerName = xaod_key,
                                     xAODContainerFrwdName = xAODContainerFrwdName)
    seq += alg
    return


def convert_photons (seq, xaod_key, key,
                     topo_cluster_xaod_key,
                     vertex_xaod_key):
    from AthenaCommon.AppMgr          import ToolSvc
    from egammaTools.EMPIDBuilderBase import EMPIDBuilderPhotonBase
    pid = EMPIDBuilderPhotonBase("photonPIDBuilder")
    ToolSvc += pid

    from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__PhotonCnvTool
    tool = xAODMaker__PhotonCnvTool (xaod_key + 'CnvTool',
                                     PIDBuilder = pid,
                                     xAODCaloClusterTopoContainerName = topo_cluster_xaod_key,
                                     xAODConversionContainerName = vertex_xaod_key)
    ToolSvc += tool
    
    from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__PhotonCnvAlg
    alg = xAODMaker__PhotonCnvAlg (xaod_key + 'Cnv',
                                   CnvTool = tool,
                                   AODContainerName = key,
                                   xAODContainerName = xaod_key)
    seq += alg
    return


def convert_muons (seq, xaod_key, key,
                   xaod_tp_key,
                   xaod_sa_key,
                   xaod_cb_key):
    #ExtrapolatedMuonTrackParticles -> xExtrapolatedMuonTrackParticles
    #StatCombinedMuonParticles -> xStatCombinedMuonParticles
    #CombinedMuonTrackParticles -> xCombinedMuonTrackParticles
    #SegmentTagTrackParticles -> xSegmentTagTrackParticles
    from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonCnvAlg
    alg = xAODMaker__MuonCnvAlg (xaod_key + 'Cnv',
                                 AODContainerName = key,
                                 xAODContainerName = xaod_key,
                                 #inDetTrackLink()
                                 #InDetTrackParticles
                                 xAODInDetTrackParticleContainerName = xaod_tp_key,
                                 #muonExtrapTrackLink
                                 #ExtrapolatedMuonSpectrometerParticles
                                 xAODSATrackParticleContainerName = xaod_sa_key,
                                 #combinedTrackLink()
                                 #CombinedFitMuonParticles
                                 xAODCombinedTrackParticleContainerName = xaod_cb_key)
    seq += alg
    return
#Muons
# Muons    TrackParticleCandidate, ExtrapolatedMuonSpectrometerParticles, CombinedfitMuonparticles
# CaloMuonCollection          ", ?, ?
# MuGirlLowBetaCollection     ?, ?, ?
# MuidMuonCollection          ", MuidExtTrackParticles, MuidCombTrackParticles
# StacoMuonCollection         ", MuonboyTrackParticles, StacoTrackParticles

#xaod
# CombinedFitMuonParticles
# ExtrapolatedMuonSpectrometerParticles
# InDetTrackParticles
# SegmentTagTrackParticles
# StatCombinedMuonParticles
#aod
#: CombinedFitMuonParticles
#: ExtrapolatedMuonSpectrometerParticles
#: MooreTrackParticles
#: MuGirlRefittedTrackParticles
#: MuTagIMOTrackParticles
#: MuTagTrackParticles
#: MuidCombTrackParticles
#: MuidExtrTrackParticles
#: MuonSpectrometerParticles
#: MuonboyMuonSpectroOnlyTrackParticles
#: MuonboyTrackParticles
#: RefittedExtrapolatedMuonSpectrometerParticles
#: ResolvedForwardTrackParticles
#: SegmentTagTrackParticles
#: StacoTrackParticles
#: StatCombinedMuonParticles
#: TrackParticleCandidate





def convert_truth (seq, xaod_key, key,
                   xaod_truth_event_key,
                   xaod_truth_vertex_key,
                   xaod_truth_links_key):
    from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
    alg = xAODMaker__xAODTruthCnvAlg \
          (xaod_key + 'Cnv',
           AODContainerName = key,
           xAODTruthParticleContainerName = xaod_key,
           xAODTruthEventContainerName = xaod_truth_event_key,
           xAODTruthVertexContainerName = xaod_truth_vertex_key,
           TruthLinks = xaod_truth_links_key)
    seq += alg
    return

types = {
    'xAOD::CaloClusterContainer' : ('CaloClusterContainer', convert_clusters),
    'xAOD::JetContainer'   :  ('JetCollection', convert_jets),
    'xAOD::TrackParticleContainer' : [('Rec::TrackParticleContainer', convert_trackparticles),
                                      ('TrackCollection', convert_tracks)],
    'xAOD::VertexContainer' : ('VxContainer', convert_vertices),
    'xAOD::ElectronContainer' : ('ElectronContainer', convert_electrons),
    'xAOD::PhotonContainer'   : ('PhotonContainer',   convert_photons),
    'xAOD::MuonContainer'     : ('Analysis::MuonContainer', convert_muons),
    'xAOD::TruthParticleContainer' : ('McEventCollection', convert_truth),
    }


def makexAOD (seq, xaod_type, xaod_key, key=None, **kw):
    if cfgKeyStore.isInInput (xaod_type, xaod_key):
        return

    if key == None:
        key = xaod_key

    tinfo_list = types[xaod_type]
    if type(tinfo_list) != type([]):
        tinfo_list = [tinfo_list]

    for tinfo in tinfo_list:
        if cfgKeyStore.isInInput (tinfo[0], key):
            if seq == None:
                seq = AlgSequence()

            tinfo[1] (seq, xaod_key, key, **kw)
            cfgKeyStore.addTransient (xaod_type, xaod_key)
            break
    return

        
def setVxLinks (seq, tpkey, vxkey):
    from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetVxLinksToTrackParticles
    alg = InDet__InDetVxLinksToTrackParticles(tpkey + 'LinkSetter',
                                              TracksName    = tpkey,
                                              VerticesName  = vxkey)
    seq += alg
    return
    
