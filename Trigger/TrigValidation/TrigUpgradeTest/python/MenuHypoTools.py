
from TrigUpgradeTest.HLTSignatureConfig import TestHypoTool, MuTestHypoTool, ElTestHypoTool
#, ElTestHypoTool, ComboTestHypoTool
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoToolInc, TrigL2CaloHypoToolMult, TrigL2ElectronHypoTool 
from TrigEgammaHypo.TrigL2CaloHypoTool import *


def TrigL2CaloHypoToolConf(name):
    from AthenaCommon.Constants import DEBUG
    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
    hypotool= TrigL2CaloHypoToolFromName(name)
    hypotool.OutputLevel = DEBUG
    return hypotool

def TrigL2ElectronHypoToolConf(name):
  from AthenaCommon.Constants import DEBUG
  from TrigEgammaHypo.TrigL2ElectronHypoTool import TrigL2ElectronHypoToolFromName
  hypotool= TrigL2ElectronHypoToolFromName(name)
  hypotool.OutputLevel = DEBUG
  return hypotool
