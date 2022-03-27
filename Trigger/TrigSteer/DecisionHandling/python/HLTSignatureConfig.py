
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from DecisionHandling.HLTSignatureHypoTools import MuTestHypoTool, ElTestHypoTool
from TriggerMenuMT.HLT.Config.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND
from AthenaConfiguration.AllConfigFlags import ConfigFlags

import sys

HLTTest__TestHypoAlg=CompFactory.getComp("HLTTest::TestHypoAlg")
HLTTest__TestRecoAlg=CompFactory.getComp("HLTTest::TestRecoAlg")



UseThisLinkName="initialRoI"
#UseThisLinkName="feature"


HLTTest__TestInputMaker=CompFactory.getComp("HLTTest::TestInputMaker")
def InputMakerForInitialRoIAlg(name):
    return HLTTest__TestInputMaker(name, RoIsLink="initialRoI", LinkName="initialRoI")

def InputMakerForFeatureAlg(name):
    return HLTTest__TestInputMaker(name, RoIsLink="initialRoI", LinkName=UseThisLinkName)


#generalize
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Configurable import Configurable

def makeSequence(ConfigFlags, name,step, signature):    
    """
    generate reco sequence for emulation chains
    """
    IM= InputMakerForFeatureAlg("IM"+signature+name+"Step"+step)
    IM.Output=name+signature+"IM"+step+"_out"
    if "el" in signature:
        Alg = CaloClustering("CaloClustering"+name+"Step"+step, FileName="emclusters.dat")
    elif "mu" in signature:
        Alg = muMSRecAlg("muMSRecAlg"+name+"Step"+step, FileName="msmu.dat")
    else:
        sys.exit("ERROR, in configuration of sequence "+name+step+signature)

    Alg.Output = name+signature+"Alg"+step+"_out"
    Alg.Input  = IM.Output
    
    if Configurable.configurableRun3Behavior == 1:         
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        accAlg = ComponentAccumulator()
        accAlg.addEventAlgo(Alg)
        InEventReco = InEventRecoCA(name+signature+"SeqStep"+step,inputMaker=IM)
        InEventReco.mergeReco(accAlg)  
            
        return (InEventReco,IM, Alg.Output)
    else:
        Sequence   = seqAND(name+signature+"SeqStep"+step, [IM, Alg])
        return (Sequence, IM, Alg.Output)




# here define the sequences from the signatures
# signatures do this:
# - declare all the RecoAlg and the HypoAlg -> create the Sequence
# - creates the InputMaker, without the inputs



#### muon signatures
#####################


def muMSRecAlg(name, FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName)

def MuHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName)

def makeMuSequence(ConfigFlags, name,step):
    return makeSequence(ConfigFlags, name,step, "mu")


## ##### electron signatures
## ##########################

def CaloClustering(name,  FileName="noreco.dat"):
    return HLTTest__TestRecoAlg(name=name, FileName=FileName)

def ElGamHypo(name):
    return HLTTest__TestHypoAlg(name=name, LinkName=UseThisLinkName)

def makeElSequence(ConfigFlags, name,step):
    return makeSequence(ConfigFlags, name,step, "el")




# Menu sequences
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequenceCA, SelectionCA, InEventRecoCA

def elMenuSequence(step, reconame, hyponame):
    (Sequence, IM, seqOut) = RecoFragmentsPool.retrieve(makeElSequence,ConfigFlags,name=reconame, step=step)
    elHypo = ElGamHypo(hyponame+"Step"+step+"ElHypo")
    elHypo.Input = seqOut
    if Configurable.configurableRun3Behavior == 1: 
        selAcc=SelectionCA(hyponame+"elStep"+step)        
        selAcc.mergeReco(Sequence) 
        selAcc.addHypoAlgo(elHypo)
        return MenuSequenceCA(selAcc, HypoToolGen=ElTestHypoTool)                 
    else:
        return MenuSequence( Maker=IM, Sequence=Sequence, Hypo=elHypo, HypoToolGen=ElTestHypoTool)
   

def gamMenuSequence(step, reconame, hyponame):
    (Sequence, IM, seqOut) = RecoFragmentsPool.retrieve(makeElSequence,ConfigFlags,name=reconame, step=step)
    elHypo = ElGamHypo(hyponame+"Step"+step+"GamHypo")
    elHypo.Input = seqOut
    if Configurable.configurableRun3Behavior == 1: 
        selAcc=SelectionCA(hyponame+"gamStep"+step+"Gam")        
        selAcc.mergeReco(Sequence) 
        selAcc.addHypoAlgo(elHypo)
        return MenuSequenceCA(selAcc, HypoToolGen=ElTestHypoTool)                 
    else:
        return MenuSequence( Maker=IM, Sequence=Sequence, Hypo=elHypo, HypoToolGen=ElTestHypoTool)
    


def muMenuSequence(step, reconame, hyponame):
    (Sequence, IM, seqOut) = RecoFragmentsPool.retrieve(makeMuSequence,ConfigFlags,name=reconame, step=step)
    muHypo = MuHypo(hyponame+"Step"+step+"MuHypo")
    muHypo.Input = seqOut
    if Configurable.configurableRun3Behavior == 1: 
        selAcc=SelectionCA(hyponame+"muStep"+step)        
        selAcc.mergeReco(Sequence) 
        selAcc.addHypoAlgo(muHypo)
        return MenuSequenceCA(selAcc, HypoToolGen=MuTestHypoTool)                 
    else:
        return MenuSequence( Maker=IM, Sequence=Sequence, Hypo=muHypo, HypoToolGen=MuTestHypoTool)
    
        
def genMenuSequence(step, reconame, hyponame):
    (Sequence, IM, seqOut) = RecoFragmentsPool.retrieve(makeElSequence,ConfigFlags,name=reconame, step=step)
    elHypo = ElGamHypo(hyponame+"Hypo")
    elHypo.Input = seqOut
    if Configurable.configurableRun3Behavior == 1: 
        selAcc=SelectionCA(hyponame+"elStep"+step)        
        selAcc.mergeReco(Sequence) 
        selAcc.addHypoAlgo(elHypo)
        return MenuSequenceCA(selAcc, HypoToolGen=ElTestHypoTool)                 
    else:
        return MenuSequence( Maker=IM, Sequence=Sequence, Hypo=elHypo, HypoToolGen=ElTestHypoTool)
    
 
