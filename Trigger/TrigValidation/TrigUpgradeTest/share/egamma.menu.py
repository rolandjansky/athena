#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


print "New egammamenu"

include("TrigUpgradeTest/testHLT_MT.py")
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
from InDetRecExample.InDetKeys import InDetKeys



from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# provide a minimal menu information
if globalflags.InputFormat.is_bytestream():
   topSequence.L1DecoderTest.ctpUnpacker.OutputLevel=DEBUG
   topSequence.L1DecoderTest.roiUnpackers[0].OutputLevel=DEBUG


from TrigUpgradeTest.HLTCFConfig import *
from TrigUpgradeTest.MenuComponents import *
from ViewAlgs.ViewAlgsConf import TestEventViewCreatorAlgorithm
from AthenaCommon.AppMgr import ServiceMgr as svcMgr


#################################
# first step: calo
#################################
#import AthenaConfiguration.ComponentAccumulator
def InputMakerAlg(name):
    from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestInputMaker
    return HLTTest__TestInputMaker(name, OutputLevel = DEBUG, LinkName="initialRoI")
 

def FastCaloAlg(name):
    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_FastAlgo
    theFastCaloAlgo=T2CaloEgamma_FastAlgo(name)
    theFastCaloAlgo.OutputLevel=VERBOSE
    theFastCaloAlgo.ClustersName="L2CaloClusters"
    svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False
    return theFastCaloAlgo

def FastCaloViewsMaker(name):
    testfastCaloViewsMaker = TestEventViewCreatorAlgorithm(name, OutputLevel=DEBUG)    
    testfastCaloViewsMaker.ViewFallThrough = True
    testfastCaloViewsMaker.RoIsLink = "initialRoI" # -||-
    testfastCaloViewsMaker.InViewRoIs = "EMCaloRoIs"
    testfastCaloViewsMaker.Views = "EMCaloViews"
    testfastCaloViewsMaker.ViewNodeName = "fastCaloInViewSubNode"
    return testfastCaloViewsMaker
  
def FastCaloHypo(name):
  from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlg
  theFastCaloHypo = TrigL2CaloHypoAlg(name)
  theFastCaloHypo.OutputLevel = DEBUG 
  return theFastCaloHypo

def TestFastCaloHypo(name):
  from TrigEgammaHypo.TrigEgammaHypoConf import TestTrigL2CaloHypoAlg
  theFastCaloHypo = TestTrigL2CaloHypoAlg(name)
  theFastCaloHypo.RunInView=True
  theFastCaloHypo.OutputLevel = DEBUG 
  return theFastCaloHypo


# Make the calo sequence
########################
theFastCaloViewsMaker = FastCaloViewsMaker(name="fastCaloViewsMaker")
theFastCaloAlgo = FastCaloAlg(name="FastCaloAlgo")

from TrigMultiVarHypo.TrigL2CaloRingerFexMTInit import init_ringer
trigL2CaloRingerFexMT = init_ringer()
trigL2CaloRingerFexMT.OutputLevel = DEBUG    


fastCaloInViewSequence = theFastCaloViewsMaker.ViewNodeName
theFastCaloHypo = TestFastCaloHypo(name="L2CaloHypo")

# Nodes & sequence
node_fastCaloViewsMaker    = AlgNode( Alg=theFastCaloViewsMaker,  inputProp='InputDecisions', outputProp='InViewRoIs')
node_fastCaloAlgo          = AlgNode( Alg=theFastCaloAlgo,        inputProp='RoIs',           outputProp='ClustersName', ViewNodeName=fastCaloInViewSequence)
node_trigL2CaloRingerFexMT = AlgNode( Alg=trigL2CaloRingerFexMT, ViewNodeName=fastCaloInViewSequence)
node_fastCaloHypo          = HypoAlgNode( Alg=theFastCaloHypo,    inputProp='CaloClusters',   outputProp='Decisions')

fastCaloViewVerify = CfgMgr.AthViews__ViewDataVerifier("FastCaloViewDataVerifier")

caloDecisionsDumper = DumpDecisions("caloDecisionsDumper", OutputLevel=DEBUG )  



fastCalo_NodeSequence = NodeSequence("fastCalo_NodeSequence",
                                         Maker=node_fastCaloViewsMaker, 
                                         Algs=[node_fastCaloAlgo], #node_trigL2CaloRingerFexMT
                                         Hypo=node_fastCaloHypo,
                                         HypoToolClassName="TrigL2CaloHypoToolConf",
                                         OtherNodes=[AlgNode(Alg=fastCaloViewVerify), AlgNode(Alg=caloDecisionsDumper)],
                                         Seed="L1EM")


caloDecisionsDumper.Decisions = theFastCaloHypo.Decisions
fastCaloViewVerify.DataObjects = [('TrigRoiDescriptorCollection' , 'StoreGateSvc+'+node_fastCaloViewsMaker.getOutput())]


#, node_trigL2CaloRingerFexMT
#EMRoIDecisions


#and then menu sequence
fastCaloSequence = MenuSequence("egammaCaloStep", nodeSeqList=[fastCalo_NodeSequence])

    
#########################################
# second step:  tracking.....
#########################################
#

from TrigUpgradeTest.InDetSetup import makeInDetAlgs
(viewAlgs, eventAlgs) = makeInDetAlgs()

# add egamma TFT
from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma
theFTF = TrigFastTrackFinder_eGamma()
theFTF.isRoI_Seeded = True
viewAlgs.append(theFTF)

# A simple algorithm to confirm that data has been inherited from parent view
# Required to satisfy data dependencies
ViewVerify = CfgMgr.AthViews__ViewDataVerifier("electronViewDataVerifier")
#ViewVerify.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+L2CaloClusters')]
viewAlgs.append(ViewVerify)

## electronViewDataVerifier = CfgMgr.AthViews__ViewDataVerifier("electronViewDataVerifier")
## electronViewDataVerifier.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+' + node_l2ElectronViewsMaker.getOutput() )]
#nodes.append(AlgNode(Alg=electronViewDataVerifier, ViewNodeName=electronInViewSequence))


TrackParticlesName = ""
for viewAlg in viewAlgs:
  if viewAlg.name() == "InDetTrigTrackParticleCreatorAlg":
    TrackParticlesName = viewAlg.TrackParticlesName


def l2ElectronViewsMaker():
    l2ElectronViewsMaker = TestEventViewCreatorAlgorithm("l2ElectronViewsMaker", OutputLevel=DEBUG)
    l2ElectronViewsMaker.ViewFallThrough = True
    l2ElectronViewsMaker.RoIsLink = "roi" # -||-
    l2ElectronViewsMaker.InViewRoIs = "EMIDRoIs" 
    l2ElectronViewsMaker.Views = "EMElectronViews"    
    l2ElectronViewsMaker.ViewNodeName = "electronInViewAlgs"
    return l2ElectronViewsMaker



def L2ElectronFex_1():
    from TrigEgammaHypo.TrigL2ElectronFexMTConfig import L2ElectronFex_1
    theElectronFex= L2ElectronFex_1()
#    theElectronFex.TrigEMClusterName = theFastCaloAlgo.ClustersName
#    theElectronFex.TrackParticlesName = InDetTrigTrackParticleCreatorAlg.TrackParticlesName
    theElectronFex.ElectronsName="Electrons"
    theElectronFex.OutputLevel=VERBOSE
    return theElectronFex




def TestTrigElectronHypo(name):
  from TrigEgammaHypo.TrigEgammaHypoConf import TestTrigL2ElectronHypoAlg
  theElectronHypo = TestTrigL2ElectronHypoAlg(name)
  theElectronHypo.RunInView=True
  theElectronHypo.OutputLevel = DEBUG 
  return theElectronHypo



#############################
# create algorithms and nodes for second step
#############################


#  alg electron:
l2ElectronViewsMaker = l2ElectronViewsMaker()
electronInViewSequence=l2ElectronViewsMaker.ViewNodeName
theElectronFex = L2ElectronFex_1()


theL2ElectronHypoAlg = TestTrigElectronHypo(name="TestTrigElectronHypoAlg")
#theFastCaloHypo #theL2ElectronHypoAlg
electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", OutputLevel=DEBUG)    



# make nodes

nodes = []
node_l2ElectronViewsMaker   = AlgNode(Alg=l2ElectronViewsMaker, inputProp='InputDecisions')
#, outputProp='InViewRoIs')
# A simple algorithm to confirm that data has been inherited from parent view
# Required to satisfy data dependencies

for alg in eventAlgs:
    nodes.append(AlgNode(alg))
    print "Added Alg %s"%(alg.name())


for viewAlg in viewAlgs:
    nodes.append(AlgNode(viewAlg, ViewNodeName=electronInViewSequence))
    print "Added Alg %s on electronInViewSequence"%(viewAlg.name())
    



node_theElectronFex   = AlgNode( Alg=theElectronFex, outputProp='ElectronsName', ViewNodeName=electronInViewSequence)
# inputProp='TrigEMClusterName'
nodes.append(node_theElectronFex)
node_TrigL2ElectronHypoAlg   = HypoAlgNode( Alg=theL2ElectronHypoAlg, inputProp='Electrons',   outputProp='Decisions')

electron_NodeSequence = NodeSequence("electron_NodeSequence",
                                        Maker=node_l2ElectronViewsMaker,                                        
                                        Algs=nodes,
                                        Hypo=node_TrigL2ElectronHypoAlg,
                                        OtherNodes=[ AlgNode(electronDecisionsDumper)],
                                        HypoToolClassName="TrigL2ElectronHypoToolConf",
                                        Seed="L1EM")

for viewAlg in viewAlgs:
    if viewAlg.properties().has_key("RoIs"):
        viewAlg.RoIs = l2ElectronViewsMaker.InViewRoIs
    if viewAlg.properties().has_key("roiCollectionName"):
        viewAlg.roiCollectionName = l2ElectronViewsMaker.InViewRoIs

theElectronFex.RoIs               = l2ElectronViewsMaker.InViewRoIs
theElectronFex.TrigEMClusterName  = theFastCaloAlgo.ClustersName
theElectronFex.TrackParticlesName = TrackParticlesName

theL2ElectronHypoAlg.Electrons = theElectronFex.ElectronsName
electronDecisionsDumper.Decisions = theL2ElectronHypoAlg.Decisions
ViewVerify.DataObjects = [('xAOD::TrigEMClusterContainer','StoreGateSvc+'+theFastCaloAlgo.ClustersName )]

electronSequence = MenuSequence("electronStep", nodeSeqList=[electron_NodeSequence])


##########################################
# menu
##########################################
#testChains = ["HLT_e3_etcut", "HLT_e5_etcut", "HLT_e7_etcut", "HLT_2e3_etcut", "HLT_e3e5_etcut"]

# map L1 decisions for menu
for unpack in topSequence.L1DecoderTest.roiUnpackers:
    if unpack.name() is "EMRoIsUnpackingTool":
        unpack.Decisions="L1EM"
    if unpack.name() is "MURoIsUnpackingTool":
        unpack.Decisions="L1MU"


testChains  = [
   Chain(name='HLT_e3_etcut', Seed="L1_EM3",   \
             ChainSteps=[ ChainStep2("Step1_e3_etcut", [fastCaloSequence]),
                          ChainStep2("Step2_e3_etcut", [electronSequence])]  ),

    ## Chain(name='HLT_e3_etcut', Seed="L1_EM3",   \
    ##         ChainSteps=[ ChainStep("Step1_e3_etcut", [SequenceHypoTool(fastCaloSequence,step1_e3_etcut() )])]  ),
 
    Chain(name='HLT_e5_etcut', Seed="L1_EM3",   \
              ChainSteps=[ChainStep2("Step1_e5_etcut", [fastCaloSequence])]),
    Chain(name='HLT_e7_etcut', Seed="L1_EM3",   \
              ChainSteps=[ChainStep2("Step1_e7_etcut", [fastCaloSequence])]),
    #Chain(name='HLT_2e3_etcut', Seed="L1_EM3",   \
    #          ChainSteps=[ChainStep("Step1_2e3_etcut", [SequenceHypoTool(fastCaloSequence, 2e3_etcut() )])])
    ]
    
topSequence.L1DecoderTest.prescaler.Prescales = ["HLT_e3_etcut:2", "HLT_2e3_etcut:2.5"]
  
## for t in theElectronHypo.HypoTools:
##   t.OutputLevel = VERBOSE
## # topSequence += theElectronHypo
## # InDetCacheCreatorTrigViews,
## electronSequence = seqAND("electronSequence", [ InDetCacheCreatorTrigViews, l2ElectronViewsMaker, electronInViewAlgs, theElectronHypo ] )

## electronDecisionsDumper = DumpDecisions("electronDecisionsDumper", OutputLevel=DEBUG, Decisions = theElectronHypo.ElectronDecisions )    
## egammaIDStep = stepSeq("egammaIDStep", filterCaloRoIsAlg, [ electronSequence,  electronDecisionsDumper ] )



##########################################
# CF construction
##########################################
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq

from DecisionHandling.DecisionHandlingConf import TriggerSummaryAlg
summary = TriggerSummaryAlg( "TriggerSummaryAlg" )
summary.InputDecision = "HLTChains"
summary.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions" ]
summary.OutputLevel = DEBUG

steps = seqAND("EgammaMenu_HLTSteps"  )
decisionTree_From_Chains(steps, testChains)
steps += summary

mon = TriggerSummaryAlg( "TriggerMonitoringAlg" )
mon.InputDecision = "HLTChains"
mon.FinalDecisions = [ "ElectronL2Decisions", "MuonL2Decisions", "WhateverElse" ]
mon.HLTSummary = "MonitoringSummary"
mon.OutputLevel = DEBUG


hltTop = seqOR( "hltTop", [ steps, mon] )
topSequence += hltTop


##########################################  


  
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)

print "Now some debug"
print theElectronFex
#print fastCaloSequence
print theFastCaloHypo
print theFastCaloViewsMaker
print l2ElectronViewsMaker
print ViewVerify

