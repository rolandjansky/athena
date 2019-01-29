#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq

def jetInputMaker( RoIs = 'FSJETRoI'):
  """ Creates the jet inputMaker"""
  
  from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
  InputMakerAlg = InputMakerForRoI("JetInputMaker", OutputLevel = DEBUG, RoIsLink="initialRoI")
  InputMakerAlg.RoIs=RoIs
  return InputMakerAlg


def jetAthSequence(ConfigFlags):
    InputMakerAlg= jetInputMaker()
    (recoSequence, sequenceOut) = jetRecoSequence(InputMakerAlg.RoIs)

    JetAthSequence =  seqAND("jetAthSequence",[InputMakerAlg, recoSequence ])
    return (JetAthSequence, InputMakerAlg, sequenceOut)

    
def jetRecoSequence(RoIs = 'FSJETRoI'):    
    # calo Cluster reco    
    from TrigT2CaloCommon.CaloDef import HLTCaloTopoRecoSequence
    jetRecoSequence = HLTCaloTopoRecoSequence(RoIs)

    from AthenaCommon.AppMgr import ToolSvc
    # Jet Reco:

    # PseudoJetAlgorithm uses a tool to convert IParticles (eg CaloClusters)
    # to PseudoJets, which are the input to FastJet. The PseudoJets are
    # stored in a PseudoJetContainer, which is written top the event store.

    from JetRec.JetRecConf import (PseudoJetAlgorithm,
                                       PseudoJetGetter)


    pseudoJetGetter = PseudoJetGetter('simpleJobPJGetter')
    pseudoJetGetter.InputContainer = 'StoreGateSvc+caloclusters'
    pseudoJetGetter.OutputContainer = 'StoreGateSvc+PseudoJetEMTopo'
    pseudoJetGetter.Label = ''
#    pseudoJetGetter.OutputLevel = DEBUG

    ToolSvc += pseudoJetGetter

    algo3 = PseudoJetAlgorithm()
#    algo3.OutputLevel = VERBOSE
    algo3.PJGetter = pseudoJetGetter

    jetRecoSequence += algo3



    # JetAlgorithm and its Tools... Reads in PseudoJetContainers,
    # alls FastJet to cluster PseudoJets,
    # and then convert the output of FastJet (new pseudojets) to Atlas jets.

    from JetRec.JetRecConf import (JetAlgorithm,
                                       JetRecTool,
                                       JetFromPseudojet,
                                       JetFinder)


    name = 'simpleJob'

    # jet from Pseudo jet takes a pseudo jet returned by FastJet

    from JetRec.JetRecConf import JetFromPseudojet  
    jetBuilder = JetFromPseudojet(name+'JetBuilder')
#    jetBuilder.OutputLevel = DEBUG
    ToolSvc += jetBuilder

    jetFinder = JetFinder(name+'JetFinder')
    jetFinder.JetBuilder = jetBuilder
#    jetFinder.OutputLevel = DEBUG
    jetFinder.JetAlgorithm = 'AntiKt'
    jetFinder.VariableRMinRadius = -1
    jetFinder.VariableRMassScale = -1
    jetFinder.GhostArea = 0.01
    jetFinder.JetRadius = 0.4
    jetFinder.PtMin = 7000. 
    jetFinder.RandomOption = 1  #  1: used run/evt number to make seed

    ToolSvc += jetFinder

    jetRecTool = JetRecTool()
    jetRecTool.InputContainer = ''  # name of a jet collection.
    jetRecTool.OutputContainer = 'jets'
    jetRecTool.JetFinder = jetFinder
    jetRecTool.JetModifiers = []
    jetRecTool.Trigger = False
#    jetRecTool.OutputLevel = DEBUG
    jetRecTool.InputPseudoJets = ['StoreGateSvc+PseudoJetEMTopo'] 

    ToolSvc += jetRecTool

    algo4 = JetAlgorithm()
    algo4.Tools = [jetRecTool]
    jetRecoSequence += algo4


    sequenceOut= jetRecTool.OutputContainer

    return (jetRecoSequence,sequenceOut)
