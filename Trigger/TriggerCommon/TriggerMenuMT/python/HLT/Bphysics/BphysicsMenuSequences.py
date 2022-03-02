# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ..Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def bmumuxAlgSequence(ConfigFlags):
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from DecisionHandling.DecisionHandlingConf import  ViewCreatorCentredOnIParticleROITool

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDConfig = getInDetTrigConfig( "bphysics" )

    viewCreatorROITool = ViewCreatorCentredOnIParticleROITool(
        RoIEtaWidth = IDConfig.etaHalfWidth,
        RoIPhiWidth = IDConfig.phiHalfWidth,
        RoisWriteHandleKey = recordable('HLT_Roi_Bmumux'))

    viewMaker = EventViewCreatorAlgorithm(
        name = 'IMbmumux',
        mergeUsingFeature = True,
        RoITool = viewCreatorROITool,
        Views = 'BmumuxViews',
        InViewRoIs = 'BmumuxViewRoIs',
        ViewFallThrough = True,
        PlaceMuonInView = True,
        InViewMuonCandidates = 'BmumuxMuonCandidates',
        InViewMuons = 'HLT_Muons_Bmumux')

    from .BphysicsRecoSequences import bmumuxRecoSequence
    recoSequence = bmumuxRecoSequence(viewMaker.InViewRoIs, viewMaker.InViewMuons)

    viewMaker.ViewNodeName = recoSequence.name()

    sequence = seqAND('bmumuxSequence', [viewMaker, recoSequence])

    return (sequence, viewMaker)


def bmumuxSequence():
    from TrigBphysHypo.TrigBphysHypoConf import TrigBphysStreamerHypo
    from TrigBphysHypo.TrigBphysStreamerHypoConfig import TrigBphysStreamerHypoToolFromDict

    sequence, viewMaker = RecoFragmentsPool.retrieve(bmumuxAlgSequence, ConfigFlags)
    hypo = TrigBphysStreamerHypo('BmumuxStreamerHypoAlg')

    return MenuSequence(
        Sequence = sequence,
        Maker = viewMaker,
        Hypo = hypo,
        HypoToolGen = TrigBphysStreamerHypoToolFromDict)


def dimuL2Sequence():
    from ..Muon.MuonMenuSequences import muCombAlgSequence
    from TrigBphysHypo.TrigBphysHypoConf import TrigBphysStreamerHypo
    from TrigBphysHypo.TrigBphysStreamerHypoConfig import TrigBphysStreamerHypoToolFromDict

    sequence, viewMaker, combinedMuonContainerName = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    hypo = TrigBphysStreamerHypo(
        name = 'DimuL2StreamerHypoAlg',
        triggerList = getNoL2CombChainNames(),
        triggerLevel = 'L2')

    return MenuSequence(
        Sequence = sequence,
        Maker = viewMaker,
        Hypo = hypo,
        HypoToolGen = TrigBphysStreamerHypoToolFromDict)


def dimuEFSequence():
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorPreviousROITool
    from TrigBphysHypo.TrigBphysHypoConf import TrigBphysStreamerHypo
    from TrigBphysHypo.TrigBphysStreamerHypoConfig import TrigBphysStreamerHypoToolFromDict

    viewMaker = InputMakerForRoI(
        name = 'IM_bphysStreamerDimuEF',
        mergeUsingFeature = True,
        RoITool = ViewCreatorPreviousROITool(),
        RoIs = 'DimuRoIs')

    sequence = seqAND('dimuSequence', [viewMaker])

    hypo = TrigBphysStreamerHypo('DimuEFStreamerHypoAlg', triggerLevel = 'EF')

    return MenuSequence(
        Sequence = sequence,
        Maker = viewMaker,
        Hypo = hypo,
        HypoToolGen = TrigBphysStreamerHypoToolFromDict)


def getNoL2CombChainNames():
    from ..Config.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()
    chains = [chain.name for chain in menu.chainsInMenu['Bphysics'] if 'noL2Comb' in chain.name]
    return chains
