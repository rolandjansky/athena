
# file to simulate the HypoTool configuration of the signatures

from AthenaCommon.Constants import VERBOSE,DEBUG

def TestHypoTool(name, prop, threshold_value):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
    value  =  int(threshold_value)*1000
    UseThisLinkName="initialRoI"
    h = HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=value, Property=prop, LinkName=UseThisLinkName)
    return h

def MuTestHypoTool(name, conf):
    threshold = conf.replace("HLT_mu","")
    return TestHypoTool(name,prop="pt", threshold_value=threshold)

def ElTestHypoTool(name,conf):
    threshold = conf.replace("HLT_e","")
    return TestHypoTool(name,prop="et", threshold_value=threshold)


def MuTest2HypoTool(name, conf):
    threshold = conf.replace("HLT_mu","")
    return TestHypoTool(name,prop="pt2", threshold_value=threshold)
