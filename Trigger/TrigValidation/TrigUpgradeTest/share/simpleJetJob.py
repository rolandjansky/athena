#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer

if TriggerFlags.doCalo:

  if ( True ) :

     from AthenaCommon.CFElements import parOR, seqAND, seqOR, stepSeq


    # menu items
     CTPToChainMapping = {
       "HLT_j85":       "L1_J20"  ,
       "HLT_j45" : "L1_J20"
     }
     testChains =[x for x, y in CTPToChainMapping.items()]
     topSequence.L1Decoder.ChainToCTPMapping = CTPToChainMapping
     print testChains

     # get L1 decisions
     for unpack in topSequence.L1Decoder.roiUnpackers:
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
       

     from TrigUpgradeTest.jetMenuHelper import jetCFSequenceFromString
     (recoSequence, InputMakerAlg, sequenceOut) = jetCFSequenceFromString("a4_tc_em_subjes")
     inputRoIs= InputMakerAlg.RoIs
     hypoDecisions= InputMakerAlg.InputMakerOutputDecisions[0]


     from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
     from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
     hypo = TrigJetHypoAlgMT("jethypo")
     hypo.Jets = sequenceOut
     hypo.HypoInputDecisions = hypoDecisions
     hypo.HypoOutputDecisions = "jetDecisions"
     
     def make_dict(chain_name):
       from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName
       chainNameDecoder = DictFromChainName.DictFromChainName()
       return chainNameDecoder.getChainDict(chain_name)
       
     hypo.HypoTools = [trigJetHypoToolFromDict(make_dict(c))
                       for c in testChains] 
     print hypo
     for tool in hypo.HypoTools:
         print tool


    
         
     ### CF construction ###
     def summarySteps ( name, decisions ):
        from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
        summarySteps = TriggerSummaryAlg( "TriggerSummary"+name )
        summarySteps.InputDecision = "L1DecoderSummary"
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
     mon.HLTTriggerMenu = list( set( MenuTest.CTPToChainMapping.keys() ) )

     hltTop = seqOR( "hltTop", [ HLTsteps, summMaker, mon ] )
     topSequence += hltTop

     from AthenaCommon.AlgSequence import dumpMasterSequence
     dumpMasterSequence()
