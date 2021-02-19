# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigBphysHypo.TrigBphysHypoConf import TrigBmumuxComboHypo, TrigBmumuxComboHypoTool
from TrigBphysHypo.TrigBmumuxComboHypoMonitoringConfig import TrigBmumuxComboHypoMonitoring, TrigBmumuxComboHypoToolMonitoring

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigBmumuxComboHypoConfig')
log.setLevel(logging.DEBUG)

def BmumuxComboHypoCfg(name):
    log.debug('BmumuxComboHypoCfg.name = %s ', name)
    suffix = 'Bmumux'

    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    vertexFitter = CompFactory.Trk__TrkVKalVrtFitter(
        name = 'TrigBphysFitter_'+suffix,
        FirstMeasuredPoint = False,
        MakeExtendedVertex = False,
        Extrapolator = AtlasExtrapolator())

    from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
    vertexPointEstimator = CompFactory.InDet__VertexPointEstimator(
        name = 'VertexPointEstimator_'+suffix,
        MinDeltaR = [-10000., -10000., -10000.],
        MaxDeltaR = [ 10000.,  10000.,  10000.],
        MaxPhi    = [ 10000.,  10000.,  10000.],
        MaxChi2OfVtxEstimation = 2000.)

    from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
    trackToVertexTool = CompFactory.Reco__TrackToVertex(
        name = 'TrackToVertexTool_'+suffix,
        Extrapolator = AtlasExtrapolator())

    hypo = CompFactory.TrigBmumuxComboHypo(
        name = 'BmumuxComboHypo',
        VertexFitter = vertexFitter,
        VertexPointEstimator = vertexPointEstimator,
        TrackToVertexTool = trackToVertexTool,
        CheckMultiplicityMap = False,
        TrigBphysCollectionKey = 'HLT_Bmumux',
        MuonCollectionKey = 'HLT_Muons_Bmumux',
        TrackCollectionKey = 'HLT_IDTrack_Bmumux_IDTrig',
        DeltaR = 0.01,
        TrkZ0 = 50.,
        MaxFitAttempts_DimuTrk1 = 200,
        MaxFitAttempts_DimuTrk1Trk2 = 2000,
        # dimuon properties
        Dimuon_rejectSameChargeTracks = True,
        Dimuon_massRange = (100., 5500.),
        Dimuon_chi2 = 20.,
        # B+ -> mu+ mu- K+
        BplusToMuMuKaon = True,
        BplusToMuMuKaon_minKaonPt = 100.,
        BplusToMuMuKaon_massRange = (4500., 5900.),
        BplusToMuMuKaon_chi2 = 50.,
        # B_c+ -> J/psi(-> mu+ mu-) pi+
        BcToMuMuPion = True,
        BcToMuMuPion_minPionPt = 2000.,
        BcToMuMuPion_dimuonMassRange = (2500., 4300.),
        BcToMuMuPion_massRange = (5500., 7300.),
        BcToMuMuPion_chi2 = 50.,
        # B_s0 -> mu+ mu- phi(-> K+ K-)
        BsToMuMuPhi1020 = True,
        BsToMuMuPhi1020_rejectSameChargeTracks = True,
        BsToMuMuPhi1020_minKaonPt = 100.,
        BsToMuMuPhi1020_massRange = (4800., 5800.),
        BsToMuMuPhi1020_phiMassRange = (940., 1100.),
        BsToMuMuPhi1020_chi2 = 60.,
        # B0 -> mu+ mu- K*0(-> K+ pi-)
        BdToMuMuKstar0 = True,
        BdToMuMuKstar0_rejectSameChargeTracks = True,
        BdToMuMuKstar0_minKaonPt = 100.,
        BdToMuMuKstar0_minPionPt = 100.,
        BdToMuMuKstar0_massRange = (4600., 5900.),
        BdToMuMuKstar0_KstarMassRange = (700., 1100.),
        BdToMuMuKstar0_chi2 = 60.,
        # Lambda_b0 -> J/psi(-> mu+ mu-) p K-
        LambdaBToMuMuProtonKaon = True,
        LambdaBToMuMuProtonKaon_rejectSameChargeTracks = False,
        LambdaBToMuMuProtonKaon_minProtonPt = 1000.,
        LambdaBToMuMuProtonKaon_minKaonPt = 1000.,
        LambdaBToMuMuProtonKaon_minKstarMass = 1300.,
        LambdaBToMuMuProtonKaon_dimuonMassRange = (2500., 4300.),
        LambdaBToMuMuProtonKaon_massRange = (4800., 6400.),
        LambdaBToMuMuProtonKaon_chi2 = 60.,
        #
        MonTool = TrigBmumuxComboHypoMonitoring('TrigBmumuxComboHypoMonitoring'))

    return hypo


def TrigBmumuxComboHypoToolFromDict(chainDict):
    config = TrigBmumuxComboHypoConfig()
    tool = config.ConfigurationComboHypoTool(chainDict)
    return tool


class TrigBmumuxComboHypoConfig(object):
    def ConfigurationComboHypoTool(self, chainDict):
        topoAlgs = chainDict['chainName']
        log.debug("Set for algorithm %s", topoAlgs)

        tool = CompFactory.TrigBmumuxComboHypoTool(topoAlgs)
        decay = chainDict['topo'][-1]
        trigDecayDict = {     # xAOD::TrigBphys::pType
            'BpmumuKp':   7,  # BKMUMU
            'BcmumuPi':  21,  # BCPIMUMU
            'BsmumuPhi':  9,  # BSPHIMUMU
            'BdmumuKst':  8,  # BDKSTMUMU
            'LbPqKm':    22,  # LBPQMUMU
        }

        tool.Decay = trigDecayDict[decay]

        tool.MonTool = TrigBmumuxComboHypoToolMonitoring('MonTool')

        return tool
