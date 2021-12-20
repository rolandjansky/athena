#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEDMConfig.TriggerEDMRun3 import recordable

def getMuonCollections (chainPart):
    from .MuonRecoSequences import muonNames
    muNames = muonNames().getNames('RoI')
    muonName = muNames.EFCBName
    if 'msonly' in chainPart['msonlyInfo']:
        muonName = muNames.EFSAName

    return muonName

def TLAMuonSequence (flags, muons):
    
    ## add the InputMaker (event context)    
    tlaMuonInputMakerAlg = CompFactory.InputMakerForRoI("IMTLAMuons"+muons)
    tlaMuonInputMakerAlg.mergeUsingFeature = True
    tlaMuonInputMakerAlg.RoITool = CompFactory.ViewCreatorPreviousROITool()
    
    tlaMuonAthSequence = seqAND( "TLAMuonAthSequence"+ muons, [tlaMuonInputMakerAlg] )
    sequenceOut = recordable(muons+"_TLA")
    return (tlaMuonAthSequence, tlaMuonInputMakerAlg, sequenceOut)


def TLAMuonMenuSequence( flags, muChainPart):
    muonsIn = getMuonCollections(muChainPart)    
    # retrieve the sequence via RecoFragmentsPool
    (tlaMuonAthSequence, tlaMuonInputMakerAlg, sequenceOut) = RecoFragmentsPool.retrieve(TLAMuonSequence, flags, muons=muonsIn)    
     #  add the hypo
    from TrigMuonHypo.TrigMuonHypoConf import TrigMuonTLAHypoAlg
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromDict
    hypo = TrigMuonTLAHypoAlg("TrigMuonTLAHypoAlg_"+muonsIn)  

    hypo.TLAOutputName = sequenceOut  

    return MenuSequence( Sequence  = tlaMuonAthSequence,
                         Maker       = tlaMuonInputMakerAlg,
                         Hypo        = hypo,
                         HypoToolGen = TrigMuonEFMSonlyHypoToolFromDict
                         )

