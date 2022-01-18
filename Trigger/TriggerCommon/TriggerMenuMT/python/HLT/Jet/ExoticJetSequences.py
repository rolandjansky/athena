# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenuMT.HLT.Jet.JetChainSequences")

from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import seqAND

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

def returnEJSequence(configflags, algo):

    return seqAND("EmergingJetsExoticSeq", [algo])

def returnCRSequence(configflags, algo):

    return seqAND("CalRatioExoticSeq", [algo])

def jetEJsMenuSequence(flags, jetsin, name):
    
    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetEJsHypoAlg
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetEJsHypoToolFromDict

    # Get track sequence name
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )
    sequenceOut  = IDTrigConfig.tracks_FTF()
    vertices     = IDTrigConfig.vertex_jet
    
    #Setup the hypothesis algorithm
    theEmergingJetsTriggerHypo           = TrigJetEJsHypoAlg("L2EmergingJets")
    theEmergingJetsTriggerHypo.Tracks    = sequenceOut
    theEmergingJetsTriggerHypo.PV        = vertices

    DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_EmergingJets_HypoOnlyStep" ) )
    DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())
    DummyInputMakerAlg.mergeUsingFeature = True

    sequence = RecoFragmentsPool.retrieve( returnEJSequence , flags, algo=DummyInputMakerAlg)


    return MenuSequence( Sequence    = sequence,
                         Maker       = DummyInputMakerAlg,
                         Hypo        = theEmergingJetsTriggerHypo,
                         HypoToolGen = trigJetEJsHypoToolFromDict,
    )
                                                                                                                            
def jetCRMenuSequence(flags, jetsin, name):

    from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetCRHypoAlg
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetCRHypoToolFromDict

    # Get track sequence name
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    from ..CommonSequences.FullScanDefs import fs_cells
    IDTrigConfig = getInDetTrigConfig( 'jet' )
    sequenceOut  = IDTrigConfig.tracks_FTF()
    cellsin=fs_cells
#    vertices     = IDTrigConfig.vertex_jet
    
    #Setup the hypothesis algorithm
    theCalRatioTriggerHypo           = TrigJetCRHypoAlg("L2CalRatio")
    theCalRatioTriggerHypo.Tracks    = sequenceOut
    theCalRatioTriggerHypo.Cells        = cellsin

    DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_CalRatio_HypoOnlyStep" ) )
    DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())
    DummyInputMakerAlg.mergeUsingFeature = True

    sequence = RecoFragmentsPool.retrieve( returnCRSequence , flags, algo=DummyInputMakerAlg)


    return MenuSequence( Sequence    = sequence,
                         Maker       = DummyInputMakerAlg,
                         Hypo        = theCalRatioTriggerHypo,
                         HypoToolGen = trigJetCRHypoToolFromDict,
    )
