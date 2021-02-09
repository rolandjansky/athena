# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.Logging import logging
log = logging.getLogger('BphysicsSequenceSetup')


def bmumuxAlgSequence(ConfigFlags):
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
    from DecisionHandling.DecisionHandlingConf import  ViewCreatorCentredOnIParticleROITool

    viewCreatorROITool = ViewCreatorCentredOnIParticleROITool(
        RoIEtaWidth = 0.75,
        RoIPhiWidth = 0.75,
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

    from TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsSetup import bmumuxRecoSequence
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


def dimuSequence():
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorPreviousROITool
    from TrigBphysHypo.TrigBphysHypoConf import TrigBphysStreamerHypo
    from TrigBphysHypo.TrigBphysStreamerHypoConfig import TrigBphysStreamerHypoToolFromDict

    inputMakerAlg = InputMakerForRoI(
        name = 'IM_bphysStreamerDimu',
        mergeUsingFeature = True,
        RoITool = ViewCreatorPreviousROITool(),
        RoIs = 'DimuRoIs')

    sequence = seqAND('dimuSequence', [inputMakerAlg])

    hypo = TrigBphysStreamerHypo('DimuStreamerHypoAlg')

    return MenuSequence(
        Sequence = sequence,
        Maker = inputMakerAlg,
        Hypo = hypo,
        HypoToolGen = TrigBphysStreamerHypoToolFromDict)
