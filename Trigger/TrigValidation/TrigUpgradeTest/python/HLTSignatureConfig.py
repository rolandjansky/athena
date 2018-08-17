from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg
from AthenaCommon.Constants import VERBOSE,DEBUG


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def InputMakerAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")


# here define the sequences from the signatures
# signatures do this:
# - declare all the RecoAlg and the HypoAlg -> create the Sequence
# - creates the InputMaker, without the inputs

from TrigUpgradeTest.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR, seqAND, stepSeq


UseThisLinkName="initialRoI"
#UseThisLinkName="feature"

#### muon signatures
#####################




def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)



muIM= InputMakerAlg(name="Step1MuInputMaker")
muIM.Output='muIM_out'

#print muIM
muAlg = muMSRecAlg(name="muMSRecAlg", FileName="msmu.dat")
muAlg.Output='muAlg_out'

muHypo = MuHypo(name="Step1MuHypo")
muAlg.Input  = muIM.Output
muHypo.Input = muAlg.Output

mustep1_sequence = seqAND("muSeqStep1", [muIM, muAlg])

def muStep1Sequence():
    return MenuSequence(Sequence=mustep1_sequence, Maker=muIM, Hypo=muHypo,HypoToolClassName="MuTestHypoTool")

# mu step2
muIM2= InputMakerAlg(name="Step2MuInputMaker")
muIM2.Output='muIM2_out'

muAlg2 = muMSRecAlg(name="muMSRecAlg2", FileName="msmu.dat")
muAlg2.Output='muAlg2_out'

muHypo2 = MuHypo(name="Step2MuHypo")
muAlg2.Input  = muIM2.Output
muHypo2.Input = muAlg2.Output

mustep2_sequence = seqAND("muSeqStep2", [muIM2, muAlg2])

def muStep2Sequence():
    return MenuSequence( Sequence=mustep2_sequence, Maker=muIM2, Hypo=muHypo2, HypoToolClassName="MuTest2HypoTool")



## ##### electron signatures
## ##########################



def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)




elIM= InputMakerAlg(name="Step1ElInputMaker")
elIM.Output='elIM_out'

elAlg = CaloClustering(name="CaloClustering", FileName="emclusters.dat")
elAlg.Output='elAlg_out'

elHypo = ElGamHypo(name="Step1ElHypo")
elAlg.Input  = elIM.Output
elHypo.Input = elAlg.Output

elstep1_sequence = seqAND("elSeqStep1", [elIM, elAlg])


def elStep1Sequence():
    return MenuSequence( Maker=elIM, Sequence=elstep1_sequence,  Hypo=elHypo, HypoToolClassName="ElTestHypoTool")

#step2
elIM2= InputMakerAlg(name="Step2ElInputMaker")
elIM2.Output='elIM2_out'

elAlg2 = CaloClustering(name="CaloClustering2", FileName="emclusters.dat")
elAlg2.Output='elAlg2_out'

elHypo2 = ElGamHypo(name="Step2ElHypo")
elAlg2.Input  = elIM2.Output
elHypo2.Input = elAlg2.Output

elstep2_sequence = seqAND("elSeqStep2", [elIM2, elAlg2])

def elStep2Sequence():
    return MenuSequence( Maker=elIM2, Sequence=elstep2_sequence, Hypo=elHypo2, HypoToolClassName="ElTestHypoTool")

