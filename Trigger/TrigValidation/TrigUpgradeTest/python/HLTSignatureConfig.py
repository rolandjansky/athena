from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestComboHypoAlg
from AthenaCommon.Constants import VERBOSE,DEBUG
from TrigUpgradeTest.HLTSignatureHypoTools import *
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import parOR, seqAND, stepSeq


UseThisLinkName="initialRoI"
#UseThisLinkName="feature"


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def InputMakerForInitialRoIAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, RoIsLink="initialRoI", LinkName="initialRoI")

def InputMakerForFeatureAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, RoIsLink="initialRoI", LinkName=UseThisLinkName)

# here define the sequences from the signatures
# signatures do this:
# - declare all the RecoAlg and the HypoAlg -> create the Sequence
# - creates the InputMaker, without the inputs

flags = ""

#### muon signatures
#####################




def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)


#class EmuStepTestNaming():
    

    
def muStep1Sequence(flags):
    muIM= InputMakerForInitialRoIAlg(name="Step1MuInputMaker")
    muIM.Output='muIM_out'

    #print muIM
    muAlg = muMSRecAlg(name="muMSRecAlg", FileName="msmu.dat")
    muAlg.Output='muAlg_out'
    muAlg.Input  = muIM.Output
    mustep1_sequence = seqAND("muSeqStep1", [muIM, muAlg])
    return (mustep1_sequence, muIM, muAlg.Output)
    
def muStep1MenuSequence():
    (mustep1_sequence, muIM, seqOut) = RecoFragmentsPool.retrieve(muStep1Sequence, flags)
    muHypo = MuHypo(name="Step1MuHypo")
    muHypo.Input = seqOut
    return MenuSequence(Sequence=mustep1_sequence, Maker=muIM, Hypo=muHypo, HypoToolGen=MuTestHypoTool)

# mu step2
def muStep2Sequence(flags):
    muIM2= InputMakerForFeatureAlg(name="Step2MuInputMaker")
    muIM2.Output='muIM2_out'

    muAlg2 = muMSRecAlg(name="muMSRecAlg2", FileName="msmu.dat")
    muAlg2.Output='muAlg2_out'

    muAlg2.Input  = muIM2.Output
    mustep2_sequence = seqAND("muSeqStep2", [muIM2, muAlg2])
    return (mustep2_sequence, muIM2, muAlg2.Output)
    
def muStep2MenuSequence():
    (mustep2_sequence, muIM2, seqOut) = RecoFragmentsPool.retrieve(muStep2Sequence, flags)
    muHypo2 = MuHypo(name="Step2MuHypo")
    muHypo2.Input =seqOut
    return MenuSequence( Sequence=mustep2_sequence, Maker=muIM2, Hypo=muHypo2, HypoToolGen=MuTest2HypoTool)



## ##### electron signatures
## ##########################




def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)



def elStep1Sequence(flags):
    elIM= InputMakerForInitialRoIAlg(name="Step1ElInputMaker")
    elIM.Output='elIM_out'

    elAlg = CaloClustering(name="CaloClustering", FileName="emclusters.dat")
    elAlg.Output='elAlg_out'
    elAlg.Input  = elIM.Output
    elstep1_sequence = seqAND("elSeqStep1", [elIM, elAlg])    
    return (elstep1_sequence, elIM, elAlg.Output)

def elStep1MenuSequence():
#    l2CaloReco = RecoFragmentsPool.retrieve(l2CaloRecoCfg, flags)
    (elstep1_sequence, elIM, seqOut) = RecoFragmentsPool.retrieve(elStep1Sequence, flags)
    elHypo = ElGamHypo(name="Step1ElHypo")
    elHypo.Input = seqOut
    return MenuSequence( Maker=elIM, Sequence=elstep1_sequence,  Hypo=elHypo, HypoToolGen=ElTestHypoTool)


def gammStep1MenuSequence():
    (elstep1_sequence, elIM, seqOut) = RecoFragmentsPool.retrieve(elStep1Sequence, flags)
    #(recoSeq, seqOut) = elStep1Sequence()
    gammHypo = ElGamHypo(name="Step1GamHypo")
    gammHypo.Input = seqOut
    return MenuSequence( Maker=elIM, Sequence=elstep1_sequence,  Hypo=gammHypo, HypoToolGen=GammTestHypoTool)


#step2
def elStep2Sequence(flags):
    elIM2= InputMakerForFeatureAlg(name="Step2ElInputMaker")
    elIM2.Output='elIM2_out'

    elAlg2 = CaloClustering(name="CaloClustering2", FileName="emclusters.dat")
    elAlg2.Output='elAlg2_out'
    elAlg2.Input  = elIM2.Output
    elstep2_sequence = seqAND("elSeqStep2", [elIM2, elAlg2])
    return (elstep2_sequence, elIM2,elAlg2.Output)
    
def elStep2MenuSequence():
    (elstep2_sequence, elIM2, seqOut) = RecoFragmentsPool.retrieve(elStep2Sequence,flags)
    elHypo2 = ElGamHypo(name="Step2ElHypo")
    elHypo2.Input = seqOut    
    return MenuSequence( Maker=elIM2, Sequence=elstep2_sequence, Hypo=elHypo2, HypoToolGen=ElTestHypoTool)

