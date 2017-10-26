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

  

# here define the sequences from the signatures
# signatures do this:
# - declare all the RecoAlg and the HypoAlg -> create the Sequence
# - creates the InputMaker, without the inputs
  

#muon signatures

def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def Step1MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, OutputLevel = DEBUG)

def mu20(name):
    return TestHypoTool(name, "pt")

def mu8(name):
    return TestHypoTool(name, "pt")


muIM= InputMakerAlg(name="Step1MuInputMaker")
step1_mu_inputMaker = AlgNode(Alg=muIM, inputProp='Input', outputProp='Output')

muAlg = muMSRecAlg(name="muMSRecAlg", FileName="msmu.dat")
step1_mu_recoAlg = AlgNode( Alg=muAlg,inputProp='Input', outputProp='Output')

muHypo = Step1MuHypo(name="Step1MuHypo")
step1_mu_HypoAlg = HypoAlgNode( Alg=muHypo,inputProp='Input', outputProp='Output')
nodeSequence_mu = NodeSequence("muNodeSeq1", InputMaker=step1_mu_inputMaker, Algs=[step1_mu_recoAlg], Hypo=step1_mu_HypoAlg, Seed="MU")

def muStep1Sequence():
    return MenuSequence("muStep1Seq", nodeSeqList=[nodeSequence_mu])


#electron signatures

def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def Step1ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, OutputLevel = DEBUG)


def e20(name):
    return TestHypoTool(name, "et")

def e8(name):
    return TestHypoTool(name, "et")

elIM= InputMakerAlg(name="Step1ElInputMaker")
step1_el_inputMaker = AlgNode(Alg=elIM, inputProp='Input', outputProp='Output')

elAlg = CaloClustering(name="CaloClustering", FileName="emclusters.dat")
step1_el_recoAlg = AlgNode( Alg=elAlg,inputProp='Input', outputProp='Output')

elHypo = Step1ElGamHypo(name="Step1ElHypo")
step1_el_HypoAlg = HypoAlgNode( Alg=elHypo,inputProp='Input', outputProp='Output')

nodeSequence_el = NodeSequence("elNodeSeq1", InputMaker=step1_el_inputMaker, Algs=[step1_el_recoAlg], Hypo=step1_el_HypoAlg, Seed="EM")
def elStep1Sequence():
    return MenuSequence("elStep1Seq", nodeSeqList=[nodeSequence_el])


#combined Hypo

def Step1ComboMuEHypo(name):
    return HLTTest__TestComboHypoAlg(name=name, OutputLevel = DEBUG, Property1="pt", Property2="et")

def mu8_e8(name):
    return TestHypoTool(name, "pt")

comboAlg = Step1ComboMuEHypo("Step1ComboMuElHypo")
step1_comb_HypoAlgMu =  HypoAlgNode( Alg=comboAlg,inputProp='Input1', outputProp='Output1')
step1_comb_HypoAlgEl =  HypoAlgNode( Alg=comboAlg,inputProp='Input2', outputProp='Output2')

nodeSequence_muComb = NodeSequence("CombmuNodeSeq1", InputMaker=step1_mu_inputMaker, Algs=[step1_mu_recoAlg], Hypo=step1_comb_HypoAlgMu, Seed="MU")
nodeSequence_elComb = NodeSequence("CombelNodeSeq1", InputMaker=step1_el_inputMaker, Algs=[step1_el_recoAlg], Hypo=step1_comb_HypoAlgEl, Seed="EM")

def combStep1Sequence():
    return MenuSequence("combStep1Seq", nodeSeqList=[nodeSequence_muComb,nodeSequence_elComb])

