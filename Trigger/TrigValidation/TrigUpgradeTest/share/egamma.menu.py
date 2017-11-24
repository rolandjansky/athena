#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

#workaround to prevent online trigger folders to be enabled
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
InDetTrigFlags.useConditionsClasses.set_Value_and_Lock(False)


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doCaloSeededBrem = False


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec = True 
InDetFlags.doPrimaryVertex3DFinding = False 
InDetFlags.doPrintConfigurables = False
InDetFlags.doResolveBackTracks = True 
InDetFlags.doSiSPSeededTrackFinder = True
InDetFlags.doTRTPhaseCalculation = True
InDetFlags.doTRTSeededTrackFinder = True
InDetFlags.doTruth = False
InDetFlags.init()

# PixelLorentzAngleSvc and SCTLorentzAngleSvc
include("InDetRecExample/InDetRecConditionsAccess.py")

viewTest = opt.enableViews   # from testHLT_MT.py
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
if viewTest:
  allViewAlgorithms = topSequence.allViewAlgorithms

from InDetRecExample.InDetKeys import InDetKeys

# provide a minimal menu information

topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG


# start here new part
#############################################################

from TrigUpgradeTest.HLTCFConfig import *
from TrigUpgradeTest.MenuComponents import *

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
def InputMakerAlg(name):
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")



def FastCaloAlg(name):
  from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
  theFastCaloAlgo=T2CaloEgamma_FastAlgo(name) #"FastCaloAlgo" )
  theFastCaloAlgo.OutputLevel=VERBOSE
  #theFastCaloAlgo.ClustersName="L2CaloClusters"
  svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False
  return theFastCaloAlgo


def FastCaloHypo(name):
  # CaloClusters ->Decisions
  from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlg
  from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromName
  theFastCaloHypo = TrigL2CaloHypoAlg(name)# "L2CaloHypo")
  theFastCaloHypo.OutputLevel = DEBUG
  #theFastCaloHypo.L1Decisions = "EMRoIDecisions"
  #theFastCaloHypo.Views = l2CaloViewsMaker.Views
  #theFastCaloHypo.CaloClusters = theFastCaloAlgo.ClustersName
  #  theFastCaloHypo.RoIs = l2CaloViewsMaker.InViewRoIs
  #theFastCaloHypo.Decisions = "EgammaCaloDecisions"
  #  theFastCaloHypo.HypoTools =  [ TrigL2CaloHypoToolFromName( c ) for c in testChains ]
  return theFastCaloHypo


def TrigL2CaloHypoTool(name):
    return TrigL2CaloHypoToolFromName(name)

def e3_etcut():
  return TrigL2CaloHypoTool("e3_etcut")

def e5_etcut():
  return TrigL2CaloHypoTool("e5_etcut")


#####################
# Make the sequence
# tmp: the InputMaker is just forwarding the filter output
filterL1RoIsAlg = RoRSeqFilter("filterL1RoIsAlg")
filterL1RoIsAlg.Input = ["EMRoIDecisions"]
filterL1RoIsAlg.Output = ["FilteredEMRoIDecisions"]
filterL1RoIsAlg.Chains = testChains
filterL1RoIsAlg.OutputLevel = DEBUG

caloIM= InputMakerAlg(name="Step1CaloInputMaker")
caloIMNode = AlgNode(Alg=caloIM, inputProp='Input', outputProp='Output') #set i/o

fastCaloAlg = FastCaloAlg(name="fastCaloAlg")
fastCaloAlgNode = AlgNode( Alg=fastCaloAlg,inputProp='RoIs', outputProp='ClustersName') 

fastCaloHypo = FastCaloHypo(name="fastCaloHypo")
fastCaloHypoNode = HypoAlgNode( Alg=fastCaloHypo,inputProp='CaloClusters', outputProp='Decisions')

fastCaloNodeSequence = NodeSequence("fastCaloNodeSequence", InputMaker=caloIMNode, Algs=[fastCaloAlgNode], Hypo=fastCaloHypoNode, Seed="EMRoIDecisions") 
fastCaloSequence = MenuSequence("fastCaloSequence", nodeSeqList=[fastCaloNodeSequence])
  #egammaCaloStep = stepSeq("egammaCaloStep", filterL1RoIsAlg, [ l2CaloViewsMaker, theFastCaloHypo, caloDecisionsDumper ])
print fastCaloSequence

# with views
# L1Decoder: RoIBResult -> [EMRoIs, EMRoIDecisions]
# filterL1RoIsAlg : EMRoIDecisions -> FilteredEMRoIDecisions
# l2CaloViewsMaker: FilteredEMRoIDecisions -> EMCaloViews
# FastCaloAlgo : EMCaloRoIs -> L2CaloClusters

# without views
# L1Decoder: RoIBResult -> [EMRoIs, EMRoIDecisions]
# filterL1RoIsAlg : EMRoIDecisions -> FilteredEMRoIDecisions
# InputMaker: FilteredEMRoIDecisions -> EMCaloRoIs
# FastCaloAlgo : EMCaloRoIs -> L2CaloClusters

# test menu
# L1Decoder: -> L1EM
# Filter_Step1L1EM_Inputs_Outputs  [L1EM] -> [Filter_Step1L1EM_L1EM_out]
# Step1ElInputMaker_Input_Output  [L1EM] -> [Step1ElInputMaker_Input_Output_out] !!!!!!
# CaloClustering_Input_Output  [Step1ElInputMaker_Input_Output_out] -> [CaloClustering_Input_Output_out] 



#filterL1RoIsAlg                                   DEBUG Data Deps for filterL1RoIsAlg
#  + INPUT  ('xAOD::TrigCompositeContainer','StoreGateSvc+EMRoIDecisions') --> from L1Decoder
#  + OUTPUT ('xAOD::TrigCompositeContainer','StoreGateSvc+FilteredEMRoIDecisions')
#l2CaloViewsMaker                                  DEBUG Data Deps for l2CaloViewsMaker
#  + INPUT  ('xAOD::TrigCompositeContainer','StoreGateSvc+FilteredEMRoIDecisions')
#  + OUTPUT ('std::vector< SG::View* >','StoreGateSvc+EMCaloViews')
#FastCaloAlgo                                      DEBUG Data Deps for FastCaloAlgo
#  + INPUT  ('TrigRoiDescriptorCollection','StoreGateSvc+EMCaloRoIs')
#  + OUTPUT ('xAOD::TrigEMClusterContainer','StoreGateSvc+L2CaloClusters')


testChains  = [
    Chain(name='HLT_e3_etcut', Seed="L1_EM3",   ChainSteps=[ChainStep("Step1_e3_etcut", [SequenceHypoTool(fastCaloSequence,"e3_etcut")])]),
    Chain(name='HLT_e5_etcut', Seed="L1_EM3",   ChainSteps=[ChainStep("Step1_e5_etcut", [SequenceHypoTool(fastCaloSequence,"e5_etcut")])])
    ]

  

decisionTree_From_Chains(topSequence, testChains, NSTEPS=1)


  ##########################################  




# CF construction



#step0 = parOR("step0", [ egammaCaloStep ] )
#step1 = parOR("step1", [ egammaIDStep ] )

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.L1Decision = "HLTChains"
summary.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions" ]
summary.OutputLevel = DEBUG

#steps = seqAND("HLTSteps", [ step0, step1, summary ]  )

mon = TriggerSummaryAlg( "TriggerMonitoringAlg" )
mon.L1Decision = "HLTChains"
mon.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions", "WhateverElse" ]
mon.HLTSummary = "MonitoringSummary"
mon.OutputLevel = DEBUG

#hltTop = seqOR( "hltTop", [ steps, mon] )
#topSequence += hltTop
  
