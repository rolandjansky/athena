# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# file to simulate the HypoTool configuration of the signatures

def TestHypoTool(name, prop, threshold_value):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
    value  =  int(threshold_value)*1000
    UseThisLinkName="initialRoI"
    h = HLTTest__TestHypoTool(name, Threshold=value, Property=prop, LinkName=UseThisLinkName)
    return h

def MuTestHypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(chainDict, 'mu') 
    return TestHypoTool(name,prop="pt", threshold_value=threshold)

def ElTestHypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(chainDict, 'e') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)

def GammTestHypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(chainDict, 'g') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)


def MuTest2HypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(chainDict, 'mu') 
    return TestHypoTool(name,prop="pt2", threshold_value=threshold)

def ElTest2HypoTool(chainDict):
    name = chainDict['chainName']
    threshold = getThreshold(chainDict, 'e') 
    return TestHypoTool(name,prop="et", threshold_value=threshold)


def getThreshold(chainDict, signature):
    name = chainDict['chainParts'][0]['chainPartName']
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getChainThresholdFromName
    return getChainThresholdFromName( name.split("_"), signature)


def dimuDrComboHypoTool(chainDict):
    from DecisionHandling.DecisionHandlingConf import DeltaRRoIComboHypoTool
    name = chainDict['chainName']
    tool= DeltaRRoIComboHypoTool(name)
    tool.DRcut=0.3
    return tool

