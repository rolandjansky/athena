
from TrigUpgradeTest.HLTSignatureConfig import TestHypoTool, MuTestHypoTool, ElTestHypoTool
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoToolInc, TrigL2CaloHypoToolMult, TrigL2ElectronHypoTool 
#from TrigEgammaHypo.TrigL2CaloHypoTool import *
from TrigMuonHypo.TrigMuonHypoConf import TrigMufastHypoTool, TrigmuCombHypoTool

def createHypoTool(hypoToolClassName, hypoToolName):
    try:
        obj=eval(hypoToolClassName)(hypoToolName)
    except:
        print "Error in creating HypoTool " + hypoToolName + " of type " + hypoToolClassName
        raise
    return obj


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


def TrigMufastHypoToolConf(name):
    from AthenaCommon.Constants import DEBUG
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigMufastHypoToolFromName
    print "in TrigMufastHypoToolConf"
    #TrigMufastHypoToolFromName
    hypotool= TrigMufastHypoToolFromName(name)
    #hypotool= TrigMufastHypoConfig.TrigMufastHypoToolFromName(name)
    hypotool.OutputLevel = DEBUG
    return hypotool

def TrigmuCombHypoToolConf(name):
    from AthenaCommon.Constants import DEBUG
    from TrigMuonHypo.testTrigMuonHypoConfig import TrigmuCombHypoToolFromName
    print "in TrigmuCombHypoToolConf"
    #TrigmuCombHypoToolFromName
    hypotool= TrigmuCombHypoToolFromName(name)
    #hypotool= TrigmuCombHypoConfig.TrigmuCombHypoToolFromName(name)
    hypotool.OutputLevel = DEBUG
    return hypotool

   
