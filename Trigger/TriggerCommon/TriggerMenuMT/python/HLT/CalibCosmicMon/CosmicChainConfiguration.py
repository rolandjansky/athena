# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)
log.setLevel(2)

from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from AthenaCommon.CFElements import seqAND
from TrigEDMConfig.TriggerEDMRun3 import recordable
import AthenaCommon.SystemOfUnits as Units

from TriggerMenuMT.HLT.Config.ChainConfigurationBase import ChainConfigurationBase

def TrackCountHypoToolGen(chainDict):
    from TrigMinBias.TrigMinBiasConf import TrackCountHypoTool
    hypo = TrackCountHypoTool(chainDict["chainName"])
    hypo.minNtrks = 1
    return hypo

def CosmicsTrkSequence(flags):
    from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlg
    from TrigInDetConfig.InDetCosmicTracking import getTrigCosmicInDetTracking
    trkInputMakerAlg, idTrackingAlgs = getTrigCosmicInDetTracking(flags)
    trackCountHypo = TrackCountHypoAlg("CosmicsTrackCountHypoAlg", 
        minPt = [100*Units.MeV],
        maxZ0 = [401*Units.mm],
        vertexZ = [803*Units.mm])
    trackCountHypo.tracksKey = recordable("HLT_IDTrack_Cosmic_IDTrig")
    trackCountHypo.trackCountKey = "HLT_CosmicsTrackCount" # irrelevant, not recorded

    #TODO move a complete configuration of the algs to TrigMinBias package
    from TrigMinBias.TrigMinBiasMonitoring import TrackCountMonitoring
    trackCountHypo.MonTool = TrackCountMonitoring(trackCountHypo) # monitoring tool configures itself using config of the hypo alg

    trkRecoSeq = parOR("CosmicTrkRecoSeq", idTrackingAlgs)
    trkSequence = seqAND("CosmicTrkSequence", [trkInputMakerAlg, trkRecoSeq])
    trkInputMakerAlg.ViewNodeName = trkRecoSeq.name()
    log.debug("Prepared ID tracking sequence")
    log.debug(trkSequence)
    return MenuSequence(Sequence    = trkSequence,
                        Maker       = trkInputMakerAlg,
                        Hypo        = trackCountHypo,
                        HypoToolGen = TrackCountHypoToolGen)

def EmptyMSBeforeCosmicID(flags):
    from TriggerMenuMT.HLT.Config.MenuComponents import EmptyMenuSequence
    return EmptyMenuSequence("EmptyBeforeCosmicID")

#----------------------------------------------------------------
class CosmicChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):       
                         
        steps = []
        log.debug("Assembling chain for %s", self.chainName)
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration         
        # --------------------
        if 'cosmic_id' in self.chainName:
            steps += [  self.getStep(1, 'Empty', [EmptyMSBeforeCosmicID]),
                        self.getStep(2, 'CosmicTracking', [CosmicsTrkSequence]) ]

        return self.buildChain(steps)


