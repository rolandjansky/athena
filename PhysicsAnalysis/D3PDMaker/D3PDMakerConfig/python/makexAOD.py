# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

    if trackmap_key is not None:
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


def convert_electrons (seq, xaod_key, key, xAODContainerFrwdName,
                       forTrigger = False):

    from AthenaCommon.AppMgr          import ToolSvc
    from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__ElectronCnvTool
    kw = {}
    if forTrigger:
        kw['xAODElectronTrackContainerName' ] = ''
        kw['xAODElectronOrigTrackContainerName' ] = ''
        kw['xAODCaloClusterContainerName' ] = ''
        kw['xAODCaloClusterSofteContainerName' ] = ''
        kw['xAODCaloClusterFrwdContainerName' ] = ''
        kw['xAODCaloClusterOtherContainerName' ] = ''

        
    tool = xAODMaker__ElectronCnvTool (xaod_key + 'CnvTool',
                                       **kw)
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
                     topo_cluster_xaod_key = '',
                     vertex_xaod_key = '',
                     forTrigger = False):
    from AthenaCommon.AppMgr          import ToolSvc

    from egammaTools.egammaToolsFactories import PhotonPIDBuilder, EMClusterTool

    from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__PhotonCnvTool
    kw = {}
    kw['xAODCaloClusterTopoContainerName'] = topo_cluster_xaod_key
    kw['xAODConversionContainerName'] = vertex_xaod_key
    if forTrigger:
        kw['xAODCaloClusterContainerName'] = ''
        kw['xAODCaloClusterOtherContainerName'] = ''
    tool = xAODMaker__PhotonCnvTool (xaod_key + 'CnvTool',
                                     PIDBuilder = PhotonPIDBuilder(),
                                     EMClusterTool = EMClusterTool(),
                                     **kw)
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



def convert_muonef (seq, xaod_key, key):

    from AthenaCommon.AppMgr          import ToolSvc
    from xAODTrigMuonCnv.xAODTrigMuonCnvConf import TrigMuonEFInfoToMuonCnvTool
    tool = TrigMuonEFInfoToMuonCnvTool \
           (xaod_key + 'CnvTool',
            xAODEFInDetTrackParticleContainerName = '')
    ToolSvc += tool

    from xAODTrigMuonCnv.xAODTrigMuonCnvConf import xAODMaker__TrigMuonEFInfoToMuonCnvAlg
    alg = xAODMaker__TrigMuonEFInfoToMuonCnvAlg \
          (xaod_key + 'Cnv',
           ConverterTool = tool,
           AODContainerName = key,
           xAODMuonContainerName = xaod_key,
           xAODCombinedTrackParticleContainerName = xaod_key + 'CombinedTrackParticles',
           xAODExtrapolatedTrackParticleContainerName = xaod_key + 'ExtrapolatedTrackParticles',
           )
    seq += alg
    return


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


def convert_met (seq, xaod_key, key):
    from xAODMissingETCnv.xAODMissingETCnvConf import xAODMaker__MissingETCnvAlg
    alg = xAODMaker__MissingETCnvAlg \
          (xaod_key + 'Cnv',
           AODKeys = [key],
           xAODKey = xaod_key)
    seq += alg
    return


def convert_trigemcluster (seq, xaod_key, key):
    from xAODTrigCaloCnv.xAODTrigCaloCnvConf import xAODMaker__TrigEMClusterCnvAlg
    alg = xAODMaker__TrigEMClusterCnvAlg \
          (xaod_key + 'Cnv',
           AODKey = key,
           xAODKey = xaod_key)
    seq += alg
    return


def convert_trigdec (seq, xaod_key, key):
    from AthenaCommon.AppMgr          import ToolSvc

    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    tdt = Trig__TrigDecisionTool ('TrigDecisionToolCnv',
                                  TrigDecisionKey = 'TrigDecision',
                                  UseAODDecision = True)
    ToolSvc += tdt
    
    from xAODTriggerCnv.xAODTriggerCnvConf import \
         xAODMaker__TrigDecisionCnvAlg, \
         xAODMaker__TrigDecisionCnvTool
    tdct = xAODMaker__TrigDecisionCnvTool (xaod_key + 'CnvTool',
                                           TrigDecisionTool = tdt)
    alg = xAODMaker__TrigDecisionCnvAlg (xaod_key + 'Cnv',
                                         AODKey = key,
                                         xAODKey = xaod_key,
                                         CnvTool = tdct)
    seq += alg
    return



types = {
    'xAOD::CaloClusterContainer' : ('CaloClusterContainer', convert_clusters),
    'xAOD::JetContainer'   :  ('JetCollection', convert_jets),
    'xAOD::TrackParticleContainer' : [('Rec::TrackParticleContainer', convert_trackparticles),
                                      ('TrackCollection', convert_tracks)],
    'xAOD::VertexContainer' : ('VxContainer', convert_vertices),
    'xAOD::ElectronContainer' : [('ElectronContainer', convert_electrons),
                                 ('egammaContainer', convert_electrons)],
    'xAOD::PhotonContainer'   : [('PhotonContainer',   convert_photons),
                                 ('egammaContainer',   convert_photons)],
    'xAOD::MuonContainer'     : [('Analysis::MuonContainer', convert_muons),
                                 ('TrigMuonEFInfoContainer', convert_muonef)],
    'xAOD::TruthParticleContainer' : ('McEventCollection', convert_truth),
    'xAOD::MissingETContainer' : ('MissingET', convert_met),
    'xAOD::TrigEMClusterContainer' : ('TrigEMClusterContainer', convert_trigemcluster),
    'xAOD::TrigDecision'      : ('TrigDec::TrigDecision', convert_trigdec),
    }


def makexAOD (seq, xaod_type, xaod_key, key=None, **kw):
    testfn = cfgKeyStore.isInInput
    if xaod_type in ['xAOD::TrigDecision']:
        testfn = cfgKeyStore.isInInputFile
    if testfn (xaod_type, xaod_key):
        return

    if key is None:
        key = xaod_key

    tinfo_list = types[xaod_type]
    if not isinstance(tinfo_list, list):
        tinfo_list = [tinfo_list]

    for tinfo in tinfo_list:
        if cfgKeyStore.isInInput (tinfo[0], key):
            if seq is None:
                seq = AlgSequence()

            tinfo[1] (seq, xaod_key, key, **kw)
            cfgKeyStore.addTransient (xaod_type, xaod_key)
            break
    return

        
