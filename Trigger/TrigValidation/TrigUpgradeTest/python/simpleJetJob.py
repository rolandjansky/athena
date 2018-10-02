#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

isData = False
if globalflags.InputFormat.is_bytestream():
  isData = True

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer

if TriggerFlags.doCalo:

  if ( True ) :
      

    # menu items
     #testChains = ["HLT_j85"]
     CTPToChainMapping = {"HLT_j85":       "L1_J20",
                            }
 
    # this is a temporary hack to include only new test chains
     testChains =[x for x, y in CTPToChainMapping.items()]
     topSequence.L1DecoderTest.ChainToCTPMapping = CTPToChainMapping
 
     print testChains
     from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
     filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
     filterL1RoIsAlg.Input = ["JRoIDecisions"]
     filterL1RoIsAlg.Output = ["FilteredJRoIDecisions"]
     filterL1RoIsAlg.Chains = testChains
     filterL1RoIsAlg.OutputLevel = DEBUG
     #topSequence += filterL1RoIsAlg
    
     from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
     InputMakerAlg = HLTTest__TestInputMaker("JetInputMaker", OutputLevel = DEBUG, LinkName="initialRoI")
     InputMakerAlg.Output='FSJETRoIs'
     InputMakerAlg.InputMakerInputDecisions = filterL1RoIsAlg.Output 
     InputMakerAlg.InputMakerOutputDecisions = ["JETRoIDecisionsOutput"]
     #topSequence += InputMakerAlg

     from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq
     jetRecoSequence = parOR("jetRecoSequence")
     
      # jets reco
     from AthenaMonitoring.GenericMonitoringTool import (GenericMonitoringTool,
                                                         defineHistogram)
     
     from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
     import math
     mon = GenericMonitoringTool("CaloDataAccessSvcMon")
     mon.Histograms += [
       defineHistogram("TIME_locking_LAr_RoI",
                       title="Time spent in unlocking the LAr collection",
                       xbins=100, xmin=0, xmax=100 ),
       defineHistogram("roiROBs_LAr",
                       title="Number of ROBs unpacked in RoI requests",
                       xbins=20, xmin=0, xmax=20 ),
      defineHistogram("TIME_locking_LAr_FullDet",
                       title="Time spent in unlocking the LAr collection",
                      xbins=100, xmin=0, xmax=100 ),
       defineHistogram("roiEta_LAr,roiPhi_LAr",
                       type="TH2F",
                       title="Geometric usage",
                       xbins=50, xmin=-5, xmax=5,
                       ybins=64, ymin=-math.pi, ymax=math.pi )]
     
     svcMgr += TrigCaloDataAccessSvc()
     svcMgr.TrigCaloDataAccessSvc.OutputLevel=INFO
     svcMgr.TrigCaloDataAccessSvc.MonTool = mon
    
     from TrigCaloRec.TrigCaloRecConf import HLTCaloCellMaker, HLTCaloCellSumMaker
     algo1=HLTCaloCellMaker("testFastAlgo1")
     algo1.RoIs=InputMakerAlg.Output
#     algo1.RoIs="StoreGateSvc+FSJETRoIs"
     algo1.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
     algo1.roiMode=False
     # algo1.roiMode=True
     algo1.OutputLevel=DEBUG
     jetRecoSequence += algo1

     from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo

     algo2 = TrigCaloClusterMakerMT_topo(doMoments=True, doLC=False)
     algo2.Cells = "StoreGateSvc+FullScanCells"
     algo2.OutputLevel = INFO
     jetRecoSequence += algo2


     # PseudoJetAlgorithm uses a tool to convert IParticles (eg CaloClusters)
     # to PseudoJets, which are the input to FastJet. The PseudoJets are
     # stored in a PseudoJetContainer, which is written top the event store.

     from JetRec.JetRecConf import (PseudoJetAlgorithm,
                                    PseudoJetGetter)

     
     pseudoJetGetter = PseudoJetGetter('simpleJobPJGetter')

     pseudoJetGetter.InputContainer = 'StoreGateSvc+caloclusters'
     pseudoJetGetter.OutputContainer = 'StoreGateSvc+PseudoJetEMTopo'
     pseudoJetGetter.Label = ''
     pseudoJetGetter.OutputLevel = DEBUG

     ToolSvc += pseudoJetGetter
     
     algo3 = PseudoJetAlgorithm()
     algo3.OutputLevel = VERBOSE
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
     jetBuilder.OutputLevel = DEBUG
     ToolSvc += jetBuilder

     jetFinder = JetFinder(name+'JetFinder')
     jetFinder.JetBuilder = jetBuilder
     jetFinder.OutputLevel = DEBUG
     jetFinder.JetAlgorithm = 'AntiKt'
     jetFinder.VariableRMinRadius = -1
     jetFinder.VariableRMassScale = -1
     jetFinder.GhostArea = 0.01
     jetFinder.JetRadius = 0.4
     jetFinder.PtMin = 70000. 
     jetFinder.RandomOption = 1  #  1: used run/evt number to make seed

     ToolSvc += jetFinder
     
     jetRecTool = JetRecTool()
     jetRecTool.InputContainer = ''  # name of a jet collection.
     jetRecTool.OutputContainer = 'StoreGateSvc+jets'
     jetRecTool.JetFinder = jetFinder
     jetRecTool.JetModifiers = []
     jetRecTool.Trigger = False
     jetRecTool.OutputLevel = DEBUG
     jetRecTool.InputPseudoJets = ['StoreGateSvc+PseudoJetEMTopo'] 

     ToolSvc += jetRecTool

     algo4 = JetAlgorithm()
     algo4.Tools = [jetRecTool]
    
     jetRecoSequence += algo4
     

     from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT, TrigJetHypoToolMT
     from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromName
  
     #TrigJetHypoToolMT
     hypo = TrigJetHypoAlgMT("jethypo")
     hypo.OutputLevel = DEBUG
     hypo.Jets = jetRecTool.OutputContainer
     hypo.HypoOutputDecisions = "jetDecisions"
     hypo.HypoInputDecisions = InputMakerAlg.InputMakerOutputDecisions[0]
     hypo.HypoTools = [ trigJetHypoToolFromName( c ) for c in testChains ] 

     #topSequence += hypo




     jetSequence = seqAND("jetSequence", [ InputMakerAlg, jetRecoSequence, hypo ])

     jetStep = stepSeq("jetStep", filterL1RoIsAlg, [ jetSequence ] )


     ### CF construction ###
     def summarySteps ( name, decisions ):
        from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
        summarySteps = TriggerSummaryAlg( "TriggerSummary"+name )
        summarySteps.InputDecision = "HLTChains"
        summarySteps.HLTSummary = "MonitoringSummary"+name
        summarySteps.OutputLevel = DEBUG
        summarySteps.FinalDecisions = decisions
        return summarySteps

     summary0 = summarySteps("Step1", [hypo.HypoOutputDecisions] )
     step0 = parOR("step0", [ jetStep, summary0 ] )
     step0filter = parOR("step0filter", [ filterL1RoIsAlg ] )
     HLTsteps = seqAND("HLTsteps", [ step0filter, step0 ]  )

  
     ### final monitor algorithm
     from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT, DecisionCollectorTool
     mon = TrigSignatureMoniMT()
     mon.FinalDecisions = [ hypo.HypoOutputDecisions ]
     from TrigUpgradeTest.TestUtils import MenuTest
     mon.ChainsList = list( set( MenuTest.CTPToChainMapping.keys() ) )
     mon.OutputLevel = DEBUG

    
     hltTop = seqOR( "hltTop", [ HLTsteps, mon ] )
     topSequence += hltTop   
