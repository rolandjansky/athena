from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg

def TestHypoTool(name, prop, threshold_value):
#    threshold_value=''.join(filter(lambda x: x.isdigit(), name))
    value  =  int(threshold_value)*1000
    h = HLTTest__TestHypoTool(name, OutputLevel=DEBUG, Threshold=value, Property=prop)
    return h

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def InputMakerAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")

  

# here define the sequences from the signatures
# signatures do this:
# - declare all the RecoAlg and the HypoAlg -> create the Sequence
# - creates the InputMaker, without the inputs
  

#muon signatures

def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, OutputLevel = DEBUG)

def step1mu20():
    return TestHypoTool("step1mu20", "pt",20)

def step1mu8():
    return TestHypoTool("step1mu8", "pt",8)

def step2mu20():
    return TestHypoTool("step2mu20", "pt", 20)

def step2mu8():
    return TestHypoTool("step2mu8", "pt", 8)


muIM= InputMakerAlg(name="Step1MuInputMaker")
step1_mu_inputMaker = AlgNode(Alg=muIM, inputProp='Inputs', outputProp='Output')

muAlg = muMSRecAlg(name="muMSRecAlg", FileName="msmu.dat")
step1_mu_recoAlg = AlgNode( Alg=muAlg,inputProp='Input', outputProp='Output')

muHypo = MuHypo(name="Step1MuHypo")

step1_mu_HypoAlg = HypoAlgNode( Alg=muHypo,inputProp='Input', outputProp='Output')
nodeSequence_mu = NodeSequence("muNodeSeq1",  Algs=[step1_mu_inputMaker, step1_mu_recoAlg], Hypo=step1_mu_HypoAlg, Seed="L1MU")

def muStep1Sequence():
    return MenuSequence("muStep1Seq", nodeSeqList=[nodeSequence_mu])

# mu step2
muIM2= InputMakerAlg(name="Step2MuInputMaker")
step2_mu_inputMaker = AlgNode(Alg=muIM2, inputProp='Inputs', outputProp='Output')

muAlg2 = muMSRecAlg(name="muMSRecAlg2", FileName="msmu.dat")
step2_mu_recoAlg = AlgNode( Alg=muAlg2,inputProp='Input', outputProp='Output')

muHypo2 = MuHypo(name="Step2MuHypo2")
step2_mu_HypoAlg = HypoAlgNode( Alg=muHypo2,inputProp='Input', outputProp='Output')
nodeSequence2_mu = NodeSequence("muNodeSeq2",  Algs=[step2_mu_inputMaker, step2_mu_recoAlg], Hypo=step2_mu_HypoAlg, Seed="")

def muStep2Sequence():
    return MenuSequence("muStep2Seq", nodeSeqList=[nodeSequence2_mu])



#electron signatures

def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, OutputLevel = DEBUG)


def step1e20():
    return TestHypoTool("step1e20", "et",20)

def step1e8():
    return TestHypoTool("step1e8", "et",8)

def step2e20():
    return TestHypoTool("step2e20", "et",20)

def step2e8():
    return TestHypoTool("step2e8", "et",8)

elIM= InputMakerAlg(name="Step1ElInputMaker")
step1_el_inputMaker = AlgNode(Alg=elIM, inputProp='Inputs', outputProp='Output')

elAlg = CaloClustering(name="CaloClustering", FileName="emclusters.dat")
step1_el_recoAlg = AlgNode( Alg=elAlg,inputProp='Input', outputProp='Output')

elHypo = ElGamHypo(name="Step1ElHypo")
step1_el_HypoAlg = HypoAlgNode( Alg=elHypo,inputProp='Input', outputProp='Output')

nodeSequence_el = NodeSequence("elNodeSeq1",  Algs=[step1_el_inputMaker, step1_el_recoAlg], Hypo=step1_el_HypoAlg, Seed="L1EM")
def elStep1Sequence():
    return MenuSequence("elStep1Seq", nodeSeqList=[nodeSequence_el])

#step2
elIM2= InputMakerAlg(name="Step2ElInputMaker")
step2_el_inputMaker = AlgNode(Alg=elIM2, inputProp='Inputs', outputProp='Output')

elAlg2 = CaloClustering(name="CaloClustering2", FileName="emclusters.dat")
step2_el_recoAlg = AlgNode( Alg=elAlg2,inputProp='Input', outputProp='Output')

elHypo2 = ElGamHypo(name="Step2ElHypo")
step2_el_HypoAlg = HypoAlgNode( Alg=elHypo2,inputProp='Input', outputProp='Output')

nodeSequence2_el = NodeSequence("elNodeSeq2",  Algs=[step2_el_inputMaker, step2_el_recoAlg], Hypo=step2_el_HypoAlg, Seed="")
def elStep2Sequence():
    return MenuSequence("elStep2Seq", nodeSeqList=[nodeSequence2_el])


# combined Hypo #
#################

def ComboMuEHypo(name):
    return HLTTest__TestComboHypoAlg(name=name, OutputLevel = DEBUG, Property1="pt", Property2="et")

def step1mu8_e8():
    return TestHypoTool("step1mu8_e8", "pt",8)

def step2mu8_e8():
    return TestHypoTool("step2mu8_e8", "pt",8)

comboAlg = ComboMuEHypo("Step1ComboMuElHypo")
step1_comb_HypoAlgMu =  HypoAlgNode( Alg=comboAlg,inputProp='Input1', outputProp='Output1')
step1_comb_HypoAlgEl =  HypoAlgNode( Alg=comboAlg,inputProp='Input2', outputProp='Output2')

nodeSequence_muComb = NodeSequence("CombmuNodeSeq1",  Algs=[step1_mu_inputMaker, step1_mu_recoAlg], Hypo=step1_comb_HypoAlgMu, Seed="L1MU")
nodeSequence_elComb = NodeSequence("CombelNodeSeq1",  Algs=[step1_el_inputMaker, step1_el_recoAlg], Hypo=step1_comb_HypoAlgEl, Seed="L1EM")


def combStep1Sequence():
    return MenuSequence("combStep1Seq", nodeSeqList=[nodeSequence_muComb,nodeSequence_elComb])

# step2

comboAlg2 = ComboMuEHypo("Step2ComboMuElHypo")
step2_comb_HypoAlgMu =  HypoAlgNode( Alg=comboAlg2,inputProp='Input1', outputProp='Output1')
step2_comb_HypoAlgEl =  HypoAlgNode( Alg=comboAlg2,inputProp='Input2', outputProp='Output2')

nodeSequence2_muComb = NodeSequence("CombmuNodeSeq2",  Algs=[step2_mu_inputMaker, step2_mu_recoAlg], Hypo=step2_comb_HypoAlgMu, Seed="")
nodeSequence2_elComb = NodeSequence("CombelNodeSeq2",  Algs=[step2_el_inputMaker, step2_el_recoAlg], Hypo=step2_comb_HypoAlgEl, Seed="")


def combStep2Sequence():
    return MenuSequence("combStep2Seq", nodeSeqList=[nodeSequence2_muComb,nodeSequence2_elComb])
