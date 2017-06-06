from TrigUpgradeTest.CFElements import *

allAlgs={}
def useExisting(name):
    global allAlgs
    return allAlgs[name]

def remember(name, instance):
    global allAlgs
    allAlgs[name] = instance
    return instance

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
def reco(name, Output,  FileName="noreco.dat", Input=""):
    a = HLTTest__TestRecoAlg("R_"+name, OutputLevel = DEBUG, FileName=FileName, Output=Output, Input=Input)
    allAlgs[name] = a
    return a

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
def hypo(name, Input, Output):
    h = HLTTest__TestHypoAlg("H_"+name, OutputLevel = DEBUG, Input=Input, Output=Output)
    allAlgs[name] = h
    return h

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter

def seqFilter(name, Inputs=[], Outputs=[], Chains=[]):
    global allAlgs
    f = HLTTest__TestRoRSeqFilter("F_"+name, OutputLevel = DEBUG, Inputs=Inputs, Outputs=Outputs, Chains=Chains)
    if "Step1" in name: # so that we see events running through, will be gone once L1 emulation is included
        f.AlwaysPass = True        
    allAlgs[name] = f
    return f

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestMerger
def merger(name, Inputs, Output ):
    return remember(name,   HLTTest__TestMerger( "M_"+name, OutputLevel=DEBUG, Inputs=Inputs, Output=Output ) )

def addSteps(s):
    return seqAND("HLTChainsSeq", s)
