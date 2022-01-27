# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLT.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLT.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
from AthenaCommon.CFElements import seqAND, parOR
from TrigGenericAlgs.TrigGenericAlgsConfig import TimeBurnerCfg, TimeBurnerHypoToolGen
from L1TopoOnlineMonitoring import L1TopoOnlineMonitoringConfig as TopoMonConfig
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.Enums import Format

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------
def TimeBurnerSequenceCfg(flags):
        # Input maker - required by the framework, but inputs don't matter for TimeBurner
        inputMaker = InputMakerForRoI("IM_TimeBurner")
        inputMaker.RoITool = ViewCreatorInitialROITool()
        inputMaker.RoIs="TimeBurnerInputRoIs"
        inputMakerSeq = seqAND("TimeBurnerSequence", [inputMaker])

        # TimeBurner alg works as a reject-all hypo
        hypoAlg = conf2toConfigurable(TimeBurnerCfg())
        hypoAlg.SleepTimeMillisec = 200

        return MenuSequence(
            Sequence    = inputMakerSeq,
            Maker       = inputMaker,
            Hypo        = hypoAlg,
            HypoToolGen = TimeBurnerHypoToolGen)

def L1TopoOnlineMonitorSequenceCfg(dummyFlags, isLegacy):
        # The menu framework actually passes None as argument to this function,
        # so use the imported ConfigFlags instead
        flags = ConfigFlags

        recoAlgCfg = TopoMonConfig.getL1TopoLegacyOnlineMonitor if isLegacy else TopoMonConfig.getL1TopoPhase1OnlineMonitor
        recoAlg = RecoFragmentsPool.retrieve(recoAlgCfg, flags)

        topoSimAlgs = []
        # if running on data without L1Sim, need to add L1TopoSim
        if flags.Input.Format is Format.BS and not flags.Trigger.doLVL1:
            topoSimAlgCfg = TopoMonConfig.getL1TopoLegacySimForOnlineMonitor if isLegacy else TopoMonConfig.getL1TopoPhase1SimForOnlineMonitor
            topoSimAlgs = RecoFragmentsPool.retrieve(topoSimAlgCfg, flags)

        # Input maker for FS initial RoI
        inputMaker = InputMakerForRoI("IM_L1TopoOnlineMonitor")
        inputMaker.RoITool = ViewCreatorInitialROITool()
        inputMaker.RoIs="L1TopoOnlineMonitorInputRoIs"

        topoMonSeqAlgs = [inputMaker]
        if topoSimAlgs:
            topoMonSeqAlgs.extend(topoSimAlgs)
        topoMonSeqAlgs.append(recoAlg)

        topoMonSeq = parOR("L1TopoOnlineMonitorSequence", topoMonSeqAlgs)

        hypoAlg = TopoMonConfig.getL1TopoOnlineMonitorHypo(flags)

        return MenuSequence(
            Sequence    = topoMonSeq,
            Maker       = inputMaker,
            Hypo        = hypoAlg,
            HypoToolGen = TopoMonConfig.L1TopoOnlineMonitorHypoToolGen)

def L1TopoLegacyOnlineMonitorSequenceCfg(flags):
    return L1TopoOnlineMonitorSequenceCfg(flags, True)

def L1TopoPhase1OnlineMonitorSequenceCfg(flags):
    return L1TopoOnlineMonitorSequenceCfg(flags, False)


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class MonitorChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):                            
        chainSteps = []
        log.debug("Assembling chain for %s", self.chainName)

        monTypeList = self.chainPart.get('monType')
        if not monTypeList:
            raise RuntimeError('No monType defined in chain ' + self.chainName)
        if len(monTypeList) > 1:
            raise RuntimeError('Multiple monType values defined in chain ' + self.chainName)
        monType = monTypeList[0]

        if monType == 'timeburner':
            chainSteps.append(self.getTimeBurnerStep())
        elif monType == 'l1topodebug':
            chainSteps.append(self.getL1TopoOnlineMonitorStep())
        else:
            raise RuntimeError('Unexpected monType '+monType+' in MonitorChainConfiguration')

        return self.buildChain(chainSteps)

    # --------------------
    # TimeBurner configuration
    # --------------------
    def getTimeBurnerStep(self):      
        return self.getStep(1,'TimeBurner',[TimeBurnerSequenceCfg])

    # --------------------
    # L1TopoOnlineMonitor configuration
    # --------------------
    def getL1TopoOnlineMonitorStep(self):
        isLegacy = 'isLegacyL1' in self.chainPart and 'legacy' in self.chainPart['isLegacyL1']
        sequenceCfg = L1TopoLegacyOnlineMonitorSequenceCfg if isLegacy else L1TopoPhase1OnlineMonitorSequenceCfg
        return self.getStep(1,'L1TopoOnlineMonitor',[sequenceCfg])
