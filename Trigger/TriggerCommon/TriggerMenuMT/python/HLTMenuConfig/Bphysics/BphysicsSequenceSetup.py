# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags


def dimuL2Sequence(name = 'Dimu'):
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muCombAlgSequence
    (l2muCombSequence, l2muCombViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(muCombAlgSequence, ConfigFlags)

    from TrigBphysHypo.TrigMultiTrkHypoConfig import TrigMultiTrkHypoConfig, TrigMultiTrkHypoToolFromDict
    config = TrigMultiTrkHypoConfig()
    DimuHypo = config.ConfigurationHypo(
        trigSequenceName = name,
        trigLevel = 'L2',
        trackCollection='HLT_IDTrack_Muon_FTF')

    return MenuSequence(
        Sequence = l2muCombSequence,
        Maker = l2muCombViewsMaker,
        Hypo = DimuHypo,
        HypoToolGen = TrigMultiTrkHypoToolFromDict)


def dimuEFSequence(name = 'Dimu'):
    from AthenaCommon import CfgMgr
    from AthenaCommon.CFElements import parOR, seqAND
    from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm, ViewCreatorInitialROITool
    from TriggerMenuMT.HLTMenuConfig.Muon.MuonSetup import muonNames

    muNames = muonNames().getNames('RoI')

    dimuefRecoSequence = parOR('dimuefViewNode')

    dimuefViewsMaker = EventViewCreatorAlgorithm('IMdimuef')
    dimuefViewsMaker.ViewFallThrough = True
    dimuefViewsMaker.RoIsLink = 'initialRoI'
    dimuefViewsMaker.RoITool = ViewCreatorInitialROITool()
    dimuefViewsMaker.InViewRoIs = 'DimuEFRoIs'
    dimuefViewsMaker.Views = 'DimuEFViewRoIs'
    dimuefViewsMaker.ViewNodeName = dimuefRecoSequence.name()
    dimuefViewsMaker.RequireParentView = True

    ViewVerifyEFCB = CfgMgr.AthViews__ViewDataVerifier('dimuefViewDataVerifier')
    ViewVerifyEFCB.DataObjects = [('xAOD::MuonContainer', 'StoreGateSvc+'+muNames.EFCBName)]
    dimuefRecoSequence += ViewVerifyEFCB
    dimuefSequence = seqAND('dimuefSequence', [dimuefViewsMaker, dimuefRecoSequence])

    from TrigBphysHypo.TrigMultiTrkHypoConfig import TrigMultiTrkHypoConfig, TrigMultiTrkHypoToolFromDict
    config = TrigMultiTrkHypoConfig()
    DimuHypo = config.ConfigurationHypo(
        trigSequenceName = name,
        trigLevel = 'EF',
        muonCollection = muNames.EFCBName)

    return MenuSequence(
        Sequence = dimuefSequence,
        Maker = dimuefViewsMaker,
        Hypo = DimuHypo,
        HypoToolGen = TrigMultiTrkHypoToolFromDict)
