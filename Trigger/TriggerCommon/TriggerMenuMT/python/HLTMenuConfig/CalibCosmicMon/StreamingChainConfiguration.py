# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.StreamingChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TrigStreamerHypo.TrigStreamerHypoConfigMT import StreamerHypoToolMTgenerator
from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from AthenaCommon.CFElements import seqAND


#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------

def StreamingSequenceCfg( flags ):    
    return StreamingMenuSequence()

def StreamingMenuSequence():

    inputMakerAlg = InputMakerForRoI("IM_streamerInputMaker")
    inputMakerAlg.RoITool = ViewCreatorInitialROITool()
    inputMakerAlg.RoIs="streamerInputRoIs"
    streamingSequence = seqAND("streamerSequence", [inputMakerAlg])

    #hypo
    streamerHypoAlg = TrigStreamerHypoAlgMT("StreamerHypoAlg")
    streamerHypoAlg.RuntimeValidation = False #Needed to avoid the ERROR ! Decision has no 'feature' ElementLink
    streamerHypoToolGen= StreamerHypoToolMTgenerator
    
    return  MenuSequence( Sequence    = streamingSequence,
                          Maker       = inputMakerAlg,
                          Hypo        = streamerHypoAlg,
                          HypoToolGen = streamerHypoToolGen )


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class StreamingChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        return self.buildChain([]) # empty list of steps and the chain requires none
