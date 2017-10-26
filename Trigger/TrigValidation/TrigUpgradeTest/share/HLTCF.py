
##### obsolete functions for ctests

from AthenaCommon.CFElements import parOR, seqAND, stepSeq
from TrigUpgradeTest.HLTCFConfig import *

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
def seqFilter(name, Inputs=[], Outputs=[], Chains=[]):
    global allAlgs
    input_list = list(set(Inputs))
    output_list = list (set(Outputs))
    chain_list = list (set(Chains))
    fname = "F_"+name
    f = HLTTest__TestRoRSeqFilter(fname, OutputLevel = DEBUG, Inputs=input_list, Outputs=output_list, Chains=chain_list)
    if "Step1" in name: # so that we see events running through, will be gone once L1 emulation is included
        f.AlwaysPass = True        
    allAlgs[name] = f
    return f

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


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestMerger
def merger(name, Inputs, Output ):
    return remember(name,   HLTTest__TestMerger( "M_"+name, OutputLevel=DEBUG, Inputs=Inputs, Output=Output ) )

def addSteps(s):
    return seqAND("HLTChainsSeq", s)





##### new

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg


def TestHypoTool(name, prop):
    threshold_value=''.join(filter(lambda x: x.isdigit(), name))
    value  =  int(threshold_value)*1000
    h = HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=value, Property=prop)
    return h

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def InputMakerAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
def SeqFilterNew(name):
    return HLTTest__TestRoRSeqFilter(name, OutputLevel = DEBUG)


def TestRecAlgNew(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def TestHypoAlgNew(name):
    return HLTTest__TestHypoAlg(name=name, OutputLevel = DEBUG)

def muTestHypoTool(name):
    return TestHypoTool(name, "pt")

def elTestHypoTool(name):
    return TestHypoTool(name, "et")

