from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg
from AthenaCommon.Constants import VERBOSE,DEBUG
from TrigUpgradeTest.HLTCFConfig import *
from TrigUpgradeTest.MenuComponents import *


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def InputMakerAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")

  

# here define the sequences from the signatures
# signatures do this:
# - declare all the RecoAlg and the HypoAlg -> create the Sequence
# - creates the InputMaker, without the inputs
  
UseThisLinkName="initialRoI"
#UseThisLinkName="feature"

#### muon signatures
#####################


def  TestHypoTool(name, prop, threshold_value):
#    threshold_value=''.join(filter(lambda x: x.isdigit(), name))
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
    value  =  int(threshold_value)*1000
    h = HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=value, Property=prop, LinkName=UseThisLinkName)
    return h

def MuTestHypoTool(name):
    threshold = name.replace("HLT_mu","")
    return TestHypoTool(name,prop="pt", threshold_value=threshold)

def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)

def MuStep1HypoTool():
   return "MuTestHypoTool"

def MuStep2HypoTool():
   return "MuTestHypoTool"



muIM= InputMakerAlg(name="Step1MuInputMaker")
step1_mu_inputMaker = AlgNode(Alg=muIM, inputProp='InputDecisions', outputProp='Output')

muAlg = muMSRecAlg(name="muMSRecAlg", FileName="msmu.dat")
step1_mu_recoAlg = AlgNode( Alg=muAlg,inputProp='Input', outputProp='Output')

muHypo = MuHypo(name="Step1MuHypo")

step1_mu_HypoAlg = HypoAlgNode( Alg=muHypo,inputProp='Input', outputProp='Output')
nodeSequence_mu = NodeSequence("muNodeSeq1",  Maker=step1_mu_inputMaker, Algs=[step1_mu_recoAlg], Hypo=step1_mu_HypoAlg, Seed="L1MU", HypoToolClassName=MuStep1HypoTool())

def muStep1Sequence():
    return MenuSequence("muStep1Seq", nodeSeqList=[nodeSequence_mu])

# mu step2
muIM2= InputMakerAlg(name="Step2MuInputMaker")
step2_mu_inputMaker = AlgNode(Alg=muIM2, inputProp='InputDecisions', outputProp='Output')

muAlg2 = muMSRecAlg(name="muMSRecAlg2", FileName="msmu.dat")
step2_mu_recoAlg = AlgNode( Alg=muAlg2,inputProp='Input', outputProp='Output')

muHypo2 = MuHypo(name="Step2MuHypo")
step2_mu_HypoAlg = HypoAlgNode( Alg=muHypo2,inputProp='Input', outputProp='Output')
nodeSequence2_mu = NodeSequence("muNodeSeq2",  Maker=step2_mu_inputMaker, Algs=[step2_mu_recoAlg], Hypo=step2_mu_HypoAlg, Seed="L1MU", HypoToolClassName=MuStep2HypoTool())

def muStep2Sequence():
    return MenuSequence("muStep2Seq", nodeSeqList=[nodeSequence2_mu])




##### electron signatures
##########################

## class ElTestHypoTool(HLTTest__TestHypoTool):
##     def __init__(self,name):
##         HLTTest__TestHypoTool.__init__(self,name, OutputLevel=DEBUG, Property="et")
##         threshold_value = name.replace("HLT_e","")
##         value  =  int(threshold_value)*1000
##         self.Threshold=value

def ElTestHypoTool(name):
    threshold = name.replace("HLT_e","")
    return TestHypoTool(name,prop="et", threshold_value=threshold)

def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)


def ElStep1HypoTool():
   return "ElTestHypoTool"

def ElStep2HypoTool():
   return "ElTestHypoTool"



elIM= InputMakerAlg(name="Step1ElInputMaker")
step1_el_inputMaker = AlgNode(Alg=elIM, inputProp='InputDecisions', outputProp='Output')

elAlg = CaloClustering(name="CaloClustering", FileName="emclusters.dat")
step1_el_recoAlg = AlgNode( Alg=elAlg,inputProp='Input', outputProp='Output')

elHypo = ElGamHypo(name="Step1ElHypo")
step1_el_HypoAlg = HypoAlgNode( Alg=elHypo,inputProp='Input', outputProp='Output')

nodeSequence_el = NodeSequence("elNodeSeq1",  Maker=step1_el_inputMaker, Algs=[step1_el_recoAlg], Hypo=step1_el_HypoAlg, Seed="L1EM", HypoToolClassName=ElStep1HypoTool())
def elStep1Sequence():
    return MenuSequence("elStep1Seq", nodeSeqList=[nodeSequence_el])

#step2
elIM2= InputMakerAlg(name="Step2ElInputMaker")
step2_el_inputMaker = AlgNode(Alg=elIM2, inputProp='InputDecisions', outputProp='Output')

elAlg2 = CaloClustering(name="CaloClustering2", FileName="emclusters.dat")
step2_el_recoAlg = AlgNode( Alg=elAlg2,inputProp='Input', outputProp='Output')

elHypo2 = ElGamHypo(name="Step2ElHypo")
step2_el_HypoAlg = HypoAlgNode( Alg=elHypo2,inputProp='Input', outputProp='Output')

nodeSequence2_el = NodeSequence("elNodeSeq2",  Maker=step2_el_inputMaker, Algs=[ step2_el_recoAlg], Hypo=step2_el_HypoAlg, Seed="L1EM",HypoToolClassName=ElStep2HypoTool())
def elStep2Sequence():
    return MenuSequence("elStep2Seq", nodeSeqList=[nodeSequence2_el])


# combined Hypo #
#################

class ComboTestHypoTool(HLTTest__TestHypoTool):
    def __init__(self,name):
        #doing nothing
        HLTTest__TestHypoTool.__init__(self,name, OutputLevel=DEBUG, Property="pt")


def ComboMuEHypo(name):
    return HLTTest__TestComboHypoAlg(name=name, OutputLevel = DEBUG, Property1="pt", Property2="et")



def ComboStep1HypoTool():
   return "ComboTestHypoTool"

def ComboStep2HypoTool():
   return "ComboTestHypoTool"

comboAlg = ComboMuEHypo("Step1ComboMuElHypo")
step1_comb_HypoAlgMu =  HypoAlgNode( Alg=comboAlg,inputProp='Input1', outputProp='Output1')
step1_comb_HypoAlgEl =  HypoAlgNode( Alg=comboAlg,inputProp='Input2', outputProp='Output2')

nodeSequence_muComb = NodeSequence("CombmuNodeSeq1",  Maker=step1_mu_inputMaker, Algs=[step1_mu_recoAlg], Hypo=step1_comb_HypoAlgMu, Seed="L1MU", HypoToolClassName=ComboStep1HypoTool())
nodeSequence_elComb = NodeSequence("CombelNodeSeq1",  Maker=step1_el_inputMaker, Algs=[step1_el_recoAlg], Hypo=step1_comb_HypoAlgEl, Seed="L1EM", HypoToolClassName=ComboStep2HypoTool())


def combStep1Sequence():
    return MenuSequence("combStep1Seq", nodeSeqList=[nodeSequence_muComb,nodeSequence_elComb])

# step2

comboAlg2 = ComboMuEHypo("Step2ComboMuElHypo")
step2_comb_HypoAlgMu =  HypoAlgNode( Alg=comboAlg2,inputProp='Input1', outputProp='Output1')
step2_comb_HypoAlgEl =  HypoAlgNode( Alg=comboAlg2,inputProp='Input2', outputProp='Output2')

nodeSequence2_muComb = NodeSequence("CombmuNodeSeq2",  Maker=step2_mu_inputMaker, Algs=[step2_mu_recoAlg], Hypo=step2_comb_HypoAlgMu, Seed="L1MU", HypoToolClassName=ComboStep1HypoTool())
nodeSequence2_elComb = NodeSequence("CombelNodeSeq2",  Maker=step2_el_inputMaker, Algs=[step2_el_recoAlg], Hypo=step2_comb_HypoAlgEl, Seed="L1EM", HypoToolClassName=ComboStep2HypoTool())


def combStep2Sequence():
    return MenuSequence("combStep2Seq", nodeSeqList=[nodeSequence2_muComb,nodeSequence2_elComb])
