#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

     topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
     topSequence.L1DecoderTest.roiUnpackers[3].OutputLevel=DEBUG

     from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq


    # menu items
     CTPToChainMapping = {"HLT_j85":       "L1_J20"   }
     testChains =[x for x, y in CTPToChainMapping.items()]
     topSequence.L1DecoderTest.ChainToCTPMapping = CTPToChainMapping
     print testChains

     #filter
     from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
     filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
     filterL1RoIsAlg.Input = ["FSJetDecisions"];
     filterL1RoIsAlg.Output = ["FilteredFSJRoIDecisions"]
     filterL1RoIsAlg.Chains = testChains
     filterL1RoIsAlg.OutputLevel = DEBUG

     #inputmaker
     from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
     InputMakerAlg = HLTTest__TestInputMaker("JetInputMaker", OutputLevel = DEBUG, LinkName="initialRoI")
     InputMakerAlg.Output='FSJETRoIs'
     InputMakerAlg.InputMakerInputDecisions = filterL1RoIsAlg.Output 
     InputMakerAlg.InputMakerOutputDecisions = ["JETRoIDecisionsOutput"]

     # reco sequence
     from TrigUpgradeTest.jetDefs import jetRecoSequence
     (recoSequence, sequenceOut) = jetRecoSequence(InputMakerAlg.Output)

     # hypo
     from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT     
     from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromName
     hypo = TrigJetHypoAlgMT("jethypo")
     hypo.OutputLevel = DEBUG
     hypo.Jets = sequenceOut
     hypo.HypoOutputDecisions = "jetDecisions"
     hypo.HypoInputDecisions = InputMakerAlg.InputMakerOutputDecisions[0]
     hypo.HypoTools = [ trigJetHypoToolFromName( c, c ) for c in testChains ] 
     print hypo
     for tool in hypo.HypoTools:
         print tool


     # finalize tree
     jetSequence = seqAND("jetSequence", [ InputMakerAlg, recoSequence, hypo ])    
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
