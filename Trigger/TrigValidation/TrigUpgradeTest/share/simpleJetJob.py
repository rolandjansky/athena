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
     CTPToChainMapping = {"HLT_j85":       "L1_J20"  , "HLT_j100" : "L1_J20" }
     testChains =[x for x, y in CTPToChainMapping.items()]
     topSequence.L1DecoderTest.ChainToCTPMapping = CTPToChainMapping
     print testChains

     # get L1 decisions
     for unpack in topSequence.L1DecoderTest.roiUnpackers:
         if unpack.name() is "JRoIsUnpackingTool":
             L1JetDecisions=unpack.Decisions
             

     inputRoIs="FSRoI"
     hypoDecisions=L1JetDecisions

     addFiltering=True
     
     if addFiltering:
         #filter
         from DecisionHandling.DecisionHandlingConf import RoRSeqFilter
         filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
         filterL1RoIsAlg.Input = [hypoDecisions]
         filterL1RoIsAlg.Output = ["FilteredL1JET"]
         filterL1RoIsAlg.Chains = testChains
         filterL1RoIsAlg.OutputLevel = DEBUG
      
         
         #inputmaker
         from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
         InputMakerAlg = InputMakerForRoI("JetInputMaker", OutputLevel = DEBUG, RoIsLink="initialRoI")
         InputMakerAlg.RoIs='FSJETRoI'
         InputMakerAlg.InputMakerInputDecisions = filterL1RoIsAlg.Output 
         InputMakerAlg.InputMakerOutputDecisions = ["JETRoIDecisionsOutput"]
         inputRoIs= InputMakerAlg.RoIs
         hypoDecisions= InputMakerAlg.InputMakerOutputDecisions[0]
         
     # get the reco sequence
     from TrigUpgradeTest.jetDefs import jetRecoSequence
     (recoSequence, sequenceOut) = jetRecoSequence(inputRoIs)

     from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
     from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromName
     hypo = TrigJetHypoAlgMT("jethypo")
     hypo.OutputLevel = DEBUG
     hypo.Jets = sequenceOut
     hypo.HypoInputDecisions = hypoDecisions
     hypo.HypoOutputDecisions = "jetDecisions"
     hypo.HypoTools = [ trigJetHypoToolFromName( c, c ) for c in testChains ] 
     print hypo
     for tool in hypo.HypoTools:
         print tool


    
         
     ### CF construction ###
     def summarySteps ( name, decisions ):
        from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
        summarySteps = TriggerSummaryAlg( "TriggerSummary"+name )
        summarySteps.InputDecision = "L1DecoderSummary"
        summarySteps.OutputLevel = DEBUG
        summarySteps.FinalDecisions = decisions
        return summarySteps


     # finalize tree
     if addFiltering:
        jetSequence = parOR("jetSequence", [ InputMakerAlg, recoSequence, hypo ])    
        jetStep = seqAND("jetStep", [filterL1RoIsAlg, jetSequence ] )

     else:
       jetStep = seqAND("jetSequence", [ recoSequence, hypo ])  


     summary0 = summarySteps("Step1", [hypo.HypoOutputDecisions] )
     step0 = seqOR("step0", [ jetStep, summary0 ] )
     if addFiltering:
        step0filter = parOR("step0filter", [ filterL1RoIsAlg ] )
        HLTsteps = seqAND("HLTsteps", [ step0filter, step0 ]  )
     else:        
        HLTsteps = seqAND("HLTsteps", [ step0 ]  )

    ### final monitor algorithm
     from TrigSteerMonitor.TrigSteerMonitorConf import TrigSignatureMoniMT, DecisionCollectorTool
     from TrigOutputHandling.TrigOutputHandlingConf import DecisionSummaryMakerAlg
     summMaker = DecisionSummaryMakerAlg()
     summMaker.FinalDecisionKeys = [ hypo.HypoOutputDecisions ]
     summMaker.FinalStepDecisions = dict.fromkeys( testChains, hypo.HypoOutputDecisions )

     mon = TrigSignatureMoniMT()
     from TrigUpgradeTest.TestUtils import MenuTest
     mon.ChainsList = list( set( MenuTest.CTPToChainMapping.keys() ) )
     mon.OutputLevel = DEBUG
    
     hltTop = seqOR( "hltTop", [ HLTsteps, summMaker, mon ] )
     topSequence += hltTop

     from AthenaCommon.AlgSequence import dumpMasterSequence
     dumpMasterSequence()
