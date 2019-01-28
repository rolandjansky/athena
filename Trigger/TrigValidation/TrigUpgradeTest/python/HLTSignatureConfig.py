from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg,  HLTTest__TestHypoAlg,  HLTTest__TestHypoTool, HLTTest__TestComboHypoAlg
from AthenaCommon.Constants import VERBOSE,DEBUG
from TrigUpgradeTest.HLTSignatureHypoTools import *
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import parOR, seqAND, stepSeq
from AthenaConfiguration.AllConfigFlags import ConfigFlags

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



#### muon signatures
#####################


def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)

    

    
def muStep1Sequence(ConfigFlags, name):
    muIM= InputMakerForInitialRoIAlg(name=name+"Step1MuInputMaker")
    muIM.Output=name+'muIM_out'

    muAlg = muMSRecAlg(name=name+"muMSRecAlg", FileName="msmu.dat")
    muAlg.Output=name+'muAlg_out'
    muAlg.Input  = muIM.Output

    mustep1_sequence = seqAND(name+"muSeqStep1", [muIM, muAlg])
    return (mustep1_sequence, muIM, muAlg.Output)
    
def muStep1MenuSequence(name):
    (mustep1_sequence, muIM, seqOut) = RecoFragmentsPool.retrieve(muStep1Sequence, ConfigFlags, name=name)
    muHypo = MuHypo(name=name+"Step1MuHypo")
    muHypo.Input = seqOut
    return MenuSequence(Sequence=mustep1_sequence, Maker=muIM, Hypo=muHypo, HypoToolGen=MuTestHypoTool)


def muStep2Sequence(ConfigFlags, name):
    muIM2= InputMakerForFeatureAlg(name=name+"Step2MuInputMaker")
    muIM2.Output=name+'muIM2_out'

    muAlg2 = muMSRecAlg(name=name+"muMSRecAlg2", FileName="msmu.dat")
    muAlg2.Output=name+'muAlg2_out'

    muAlg2.Input  = muIM2.Output
    mustep2_sequence = seqAND(name+"muSeqStep2", [muIM2, muAlg2])
    return (mustep2_sequence, muIM2, muAlg2.Output)
    
def muStep2MenuSequence(name):
    (mustep2_sequence, muIM2, seqOut) = RecoFragmentsPool.retrieve(muStep2Sequence, ConfigFlags, name=name)
    muHypo2 = MuHypo(name=name+"Step2MuHypo")
    muHypo2.Input =seqOut
    return MenuSequence( Sequence=mustep2_sequence, Maker=muIM2, Hypo=muHypo2, HypoToolGen=MuTest2HypoTool)



## ##### electron signatures
## ##########################

def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName, OutputLevel = DEBUG)

def ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName, OutputLevel = DEBUG)



def elStep1Sequence(ConfigFlags, name):
    elIM= InputMakerForInitialRoIAlg(name=name+"Step1ElInputMaker")
    elIM.Output=name+'elIM_out'

    elAlg = CaloClustering(name=name+"CaloClustering", FileName="emclusters.dat")
    elAlg.Output=name+'elAlg_out'
    elAlg.Input  = elIM.Output
    elstep1_sequence = seqAND(name+"elSeqStep1", [elIM, elAlg])    
    return (elstep1_sequence, elIM, elAlg.Output)

def elStep1MenuSequence(name):
    (elstep1_sequence, elIM, seqOut) = RecoFragmentsPool.retrieve(elStep1Sequence, ConfigFlags, name=name)
    elHypo = ElGamHypo(name=name+"Step1ElHypo")
    elHypo.Input = seqOut
    return MenuSequence( Maker=elIM, Sequence=elstep1_sequence,  Hypo=elHypo, HypoToolGen=ElTestHypoTool)


def gammStep1MenuSequence(name):
    (elstep1_sequence, elIM, seqOut) = RecoFragmentsPool.retrieve(elStep1Sequence, ConfigFlags, name=name)
    gammHypo = ElGamHypo(name=name+"Step1GamHypo")
    gammHypo.Input = seqOut
    return MenuSequence( Maker=elIM, Sequence=elstep1_sequence,  Hypo=gammHypo, HypoToolGen=GammTestHypoTool)


#step2
def elStep2Sequence(ConfigFlags, name):
#    print "FPDEBUG Running elStep2Sequence with flags %s and name %s"%(ConfigFlags, name)
    elIM2= InputMakerForFeatureAlg(name=name+"Step2ElInputMaker")
    elIM2.Output=name+'elIM2_out'

    elAlg2 = CaloClustering(name=name+"CaloClustering2", FileName="emclusters.dat")
    elAlg2.Output=name+'elAlg2_out'
    elAlg2.Input  = elIM2.Output
    elstep2_sequence = seqAND(name+"elSeqStep2", [elIM2, elAlg2])
    return (elstep2_sequence, elIM2,elAlg2.Output)
    
def elStep2MenuSequence(name):
    (elstep2_sequence, elIM2, seqOut) = RecoFragmentsPool.retrieve(elStep2Sequence,ConfigFlags,name=name)
    elHypo2 = ElGamHypo(name+"Step2ElHypo")
    elHypo2.Input = seqOut    
    return MenuSequence( Maker=elIM2, Sequence=elstep2_sequence, Hypo=elHypo2, HypoToolGen=ElTestHypoTool)


