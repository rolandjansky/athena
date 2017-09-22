
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
def TestRecoAlg(name, Output,  FileName="noreco.dat", Input=""):
    fname = "R_"+name
    a = HLTTest__TestRecoAlg(fname, OutputLevel = DEBUG, FileName=FileName, Output=Output, Input=Input)
    allAlgs[name] = a
    return a


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
def TestHypoAlg(name, Input, Output):
    fname = "H_"+name
    h = HLTTest__TestHypoAlg(fname, OutputLevel = DEBUG, Input=Input, Output=Output)
    allAlgs[name] = h
    return h

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
def TestHypoTool(name, prop):
    threshold_value=''.join(filter(lambda x: x.isdigit(), name))
    value  =  int(threshold_value)*1000
    h = HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=value, Property=prop)
    return h

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg
def TestComboHypoAlg(name, Input, Output, prop1, prop2):
    ## TMP
    fname = "H_"+name
    h = HLTTest__TestComboHypoAlg(fname, OutputLevel = DEBUG, Input1=Input[0], Input2=Input[1], \
                                  Output1=Output[0],Output2=Output[1],\
                                  Property1=prop1, Property2=prop2)
                                  #, Threshold1=8000, Threshold2=8000,\
                                  #DecisionLabel="HLT_mu8_e8")
    allAlgs[name] = h
    return h


#muon signatures

def muMSRecAlg(name, Output,  FileName="noreco.dat", Input=""):    
    return TestRecoAlg(name, Output,  FileName, Input)

def Step1MuHypo(name, Input, Output):
    return TestHypoAlg(name, Input, Output)

def mu20(name):
    return TestHypoTool(name, "pt")

def mu8(name):
    return TestHypoTool(name, "pt")


#electron signatures

def CaloClustering(name, Output,  FileName="noreco.dat", Input=""):    
    return TestRecoAlg(name, Output,  FileName, Input)

def Step1ElGamHypo(name, Input, Output):
    return TestHypoAlg(name, Input, Output)

def e20(name):
    return TestHypoTool(name, "et")

def e8(name):
    return TestHypoTool(name, "et")


#combined Hypo

def Step1ComboMuEHypo(name, Input, Output):
    return TestComboHypoAlg(name, Input, Output, "pt", "et")

def mu8_e8(name):
    return TestHypoTool(name, "pt")
