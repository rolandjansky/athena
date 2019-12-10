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

     testChains = ["HLT_j85_L1J20", "HLT_j45_L1J20"]
     from TrigUpgradeTest.TestUtils import makeChain
     for c in  testChains:
       makeChain(  name=c, L1Thresholds=["J20"], ChainSteps=[])

     from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
     generateJSON()

     from TrigConfigSvc.TrigConfigSvcCfg import getHLTConfigSvc
     svcMgr += getHLTConfigSvc()

     # get L1 decisins
     for unpack in topSequence.L1Decoder.roiUnpackers:
         if unpack.name() is "JRoIsUnpackingTool":
             L1JetDecisions=unpack.Decisions

     from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
     inputRoIs=mapThresholdToL1RoICollection("FS")
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
     InputMakerAlg.InputMakerInputDecisions = filterL1RoIsAlg.Output
     InputMakerAlg.InputMakerOutputDecisions = [ x+"Out" for x in InputMakerAlg.InputMakerInputDecisions ]
     inputRoIs= InputMakerAlg.RoIs
     hypoDecisions= InputMakerAlg.InputMakerOutputDecisions[0]


     from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoAlgMT
     from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolFromDict
     hypo = TrigJetHypoAlgMT("jethypo")
     hypo.Jets = sequenceOut
     hypo.HypoInputDecisions = hypoDecisions
     hypo.HypoOutputDecisions = "jetDecisions"

     from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
     hypo.HypoTools = [trigJetHypoToolFromDict(c)
                       for c in TriggerConfigHLT.dicts().values()]
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

     hltTop = seqOR( "hltTop", [ HLTsteps, summMaker, mon ] )
     topSequence += hltTop

     from AthenaCommon.AlgSequence import dumpMasterSequence
     dumpMasterSequence()
