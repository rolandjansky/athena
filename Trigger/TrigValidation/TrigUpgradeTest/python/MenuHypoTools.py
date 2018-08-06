
from TrigUpgradeTest.HLTSignatureHypoTools import TestHypoTool, MuTestHypoTool, ElTestHypoTool, MuTest2HypoTool
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoToolInc, TrigL2CaloHypoToolMult, TrigL2ElectronHypoTool, TrigL2PhotonHypoTool 
from TrigMuonHypo.TrigMuonHypoConf import TrigMufastHypoTool, TrigmuCombHypoTool
from AthenaCommon.Constants import VERBOSE,INFO,DEBUG


# HypoTool name is the chain name (can be combined)
# HypoTool conf is a string decondgin the actual configuration (not combined)

def createHypoTool(hypoToolClassName, hypoToolName, hypoToolConf):
    try:
        obj=eval(hypoToolClassName)(hypoToolName, hypoToolConf)
    except:
        print "Error in creating HypoTool " + hypoToolName + " of type " + hypoToolClassName + " with conf " + hypoToolConf
        raise
    return obj


def TrigL2CaloHypoToolConf(name, conf):
    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
    hypotool= TrigL2CaloHypoToolFromName(name,conf)
    hypotool.OutputLevel = DEBUG
    return hypotool

def TrigL2ElectronHypoToolConf(name,conf):
  from TrigEgammaHypo.TrigL2ElectronHypoTool import TrigL2ElectronHypoToolFromName
  hypotool= TrigL2ElectronHypoToolFromName(name,conf)
  hypotool.OutputLevel = DEBUG
  return hypotool

def TrigL2PhotonHypoToolConf(name,conf):
  from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromName
  hypotool= TrigL2PhotonHypoToolFromName(name,conf)
  hypotool.OutputLevel = VERBOSE
  return hypotool


def TrigMufastHypoToolConf(name, conf):
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigMufastHypoToolFromName
    hypotool= TrigMufastHypoToolFromName(name, conf)
    hypotool.OutputLevel = DEBUG
    return hypotool

def TrigmuCombHypoToolConf(name, conf):
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigmuCombHypoToolFromName
    hypotool= TrigmuCombHypoToolFromName(name, conf)
    hypotool.OutputLevel = DEBUG
    return hypotool

   
