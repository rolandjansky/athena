
from TrigUpgradeTest.HLTSignatureConfig import TestHypoTool, MuTestHypoTool, ElTestHypoTool
from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoToolInc, TrigL2CaloHypoToolMult, TrigL2ElectronHypoTool 
#from TrigEgammaHypo.TrigL2CaloHypoTool import *
from TrigMuonHypo.TrigMuonHypoConf import TrigMufastHypoTool

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



def TrigJetHypoToolConf(name):
    from AthenaCommon.Constants import DEBUG
    from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromName
    try:
        hypotool= trigJetHypoToolFromName(name)
    except Exception, e:
        print e
        raise e
    #hypotool= TrigMufastHypoConfig.TrigMufastHypoToolFromName(name)
    hypotool.OutputLevel = DEBUG
    return hypotool

if __name__ == '__main__':
    print TrigJetHypoToolConf('HLT_j85')
