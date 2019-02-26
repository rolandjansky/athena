
# file to simulate the HypoTool configuration of the signatures

from AthenaCommon.Constants import VERBOSE,DEBUG

def TestHypoTool(name, prop, threshold_value):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
    value  =  int(threshold_value)*1000
    UseThisLinkName="initialRoI"
    h = HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=value, Property=prop, LinkName=UseThisLinkName)
    return h

def MuTestHypoTool(chainDict):
    #threshold = conf.replace("HLT_mu","")
    conf = name = chainDict['chainName']
    threshold = getThreshold(conf, 'mu') 
    return TestHypoTool(name,prop="pt", threshold_value=threshold)

def ElTestHypoTool(chainDict):
    #threshold = conf.replace("HLT_e","")
    conf = name = chainDict['chainName']
    threshold = getThreshold(conf, 'e') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)

def GammTestHypoTool(chainDict):
    conf = name = chainDict['chainName']
    threshold = getThreshold(conf, 'g') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)


def MuTest2HypoTool(chainDict):
    conf = name = chainDict['chainName']
    threshold = getThreshold(conf, 'mu')  
    return TestHypoTool(name,prop="pt2", threshold_value=threshold)


def getThreshold(name, signature):
    from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName
    dictDecoding = DictFromChainName.DictFromChainName()
    return dictDecoding.getChainThresholdFromName(name, signature)
