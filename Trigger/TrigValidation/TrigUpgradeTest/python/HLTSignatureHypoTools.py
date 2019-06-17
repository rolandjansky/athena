
# file to simulate the HypoTool configuration of the signatures

def TestHypoTool(name, prop, threshold_value):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
    value  =  int(threshold_value)*1000
    UseThisLinkName="initialRoI"
    h = HLTTest__TestHypoTool(name, Threshold=value, Property=prop, LinkName=UseThisLinkName)
    return h

def MuTestHypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(name, 'mu') 
    return TestHypoTool(name,prop="pt", threshold_value=threshold)

def ElTestHypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(name, 'e') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)

def GammTestHypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(name, 'g') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)


def MuTest2HypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(name, 'mu')  
    return TestHypoTool(name,prop="pt2", threshold_value=threshold)

def ElTest2HypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(name, 'e') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)


def getThreshold(name, signature):
    from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName
    dictDecoding = DictFromChainName.DictFromChainName()
    return dictDecoding.getChainThresholdFromName(name, signature)
