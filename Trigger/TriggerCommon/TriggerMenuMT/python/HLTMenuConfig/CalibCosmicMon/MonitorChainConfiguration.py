# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.MonitorChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, MenuSequence
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from AthenaCommon.CFElements import seqAND
from TrigGenericAlgs.TrigGenericAlgsConfig import TimeBurnerCfg, TimeBurnerHypoToolGen
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class MonitorChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)

        if self.chainPartName == 'costmonitor':
            pass  # costmonitor is a streamer so has no steps
        elif self.chainPartName == 'timeburner':
            chainSteps.append(self.getTimeBurnerStep())
        else:
            raise RuntimeError('Unexpected chainPartName '+self.chainPartName+' in MonitorChainConfiguration')

        return self.buildChain(chainSteps)

    # --------------------
    # TimeBurner configuration
    # --------------------
    def getTimeBurnerStep(self):
        # Input maker - required by the framework, but inputs don't matter for TimeBurner
        inputMaker = InputMakerForRoI("IM_TimeBurner")
        inputMaker.RoITool = ViewCreatorInitialROITool()
        inputMaker.RoIs="TimeBurnerInputRoIs"
        inputMakerSeq = seqAND("TimeBurnerSequence", [inputMaker])

        # TimeBurner alg works as a reject-all hypo
        hypoAlg = conf2toConfigurable(TimeBurnerCfg())
        hypoAlg.SleepTimeMillisec = 200

        seq = MenuSequence(
            Sequence    = inputMakerSeq,
            Maker       = inputMaker,
            Hypo        = hypoAlg,
            HypoToolGen = TimeBurnerHypoToolGen)

        return ChainStep(name='Step1_TimeBurner', Sequences=[seq])
