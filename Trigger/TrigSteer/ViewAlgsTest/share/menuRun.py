#
# get_files -xmls LVL1config_Physics_pp_v5.xml (or acquire it by some other means)
# 
# 

## @file L1Topo_ReadBS_test.py
## @brief Example job options file to read BS file to test a converter
## $Id: $
###############################################################
#
# This Job option:
# ----------------
# 1. Read ByteStream test data file and decode the L1Topo part
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


#define way of running with or without Gaudi sequneces:
runGaudiSequence=False
if runGaudiSequence:
    from AthenaCommon.AlgSequence import AthSequencer
    job += AthSequencer("MySeq")
    topSequence = job.MySeq
else:
    topSequence = job



#topSequence = AlgSequence()




from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
from TriggerJobOpts.TriggerFlags import TriggerFlags

l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())
svcMgr += l1svc


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from ViewAlgsTest.Menu import *
from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO, connectHypoToMenu, graph_generator

#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
caloFakeRoI = FakeRoI("caloFakeRoI")
caloFakeRoI.InputFilename="caloRoIData.dat"
setOutput(caloFakeRoI, "OutputDecisions", "CaloRoIs")
setOutput(caloFakeRoI, "OutputRoIs", "CaloTrigRoIs")
setOutput(caloFakeRoI, "OutputRecEMTauRoIs", "CaloRecRoIs")


# from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
# setTHistSvcOutput([])
# if not hasattr(ServiceMgr, 'THistSvc'):       
#     from GaudiSvc.GaudiSvcConf import THistSvc
#     ServiceMgr += THistSvc()
#     if hasattr(ServiceMgr.THistSvc, "Output"): # this is offline THistSvc fo which we want to setup files
#         setTHistSvcOutput(ServiceMgr.THistSvc.Output)

# from TrigMonitorBase.TrigGenericMonitoringToolConfig import *
# mt = TrigGenericMonitoringTool("CaloRoIsMonitor")
# mt.OutputLevel=VERBOSE
# mt.Histograms += [defineHistogram("roiCount", path="EXPERT", type="TH1F", title="RoIs count;#RoIs;events", xbins=30, xmin=0, xmax=30)]
# caloFakeRoI.monTools += [mt]



caloFakeRoI.OutputLevel=DEBUG
topSequence += caloFakeRoI

# Test the execution of an algorithm within an event view
testViewAlgorithm = True
if ( testViewAlgorithm ):

  # A sequence to hide view algorithms from the event context
  from AthenaCommon.AlgSequence import AthSequencer
  allViewAlgorithms = AthSequencer( "allViewAlgorithms" )
  allViewAlgorithms += CfgMgr.AthPrescaler( "alwaysFail" )
  allViewAlgorithms.alwaysFail.PercentPass = 0.0

  # The algorithm to run inside the views
  from ViewAlgsTest.ViewAlgsTestConf import SchedulerProxyAlg
  viewAlgName = "algInView"
  viewAlg = SchedulerProxyAlg( viewAlgName )
  viewAlg.RoIsContainer = "InViewRoI"
  allViewAlgorithms += viewAlg

  # The algorithm to launch the views
  from ViewAlgsTest.ViewAlgsTestConf import TestViewDriver
  runInViews = TestViewDriver( "runInViews" )
  connectAlgorithmsIO ( producer=(caloFakeRoI, "OutputRoIs"), consumer=(runInViews, "RoIsContainer") ) 
  runInViews.OutputLevel = DEBUG
  runInViews.ViewNodeName = "allViewAlgorithms"
  runInViews.RoIsViewOutput = "InViewRoI"
  runInViews.Scheduler = AlgScheduler.getScheduler()
  topSequence += runInViews
  topSequence += allViewAlgorithms


# muonFakeRoI = FakeRoI("muonFakeRoI")
# muonFakeRoI.InputFilename="muonRoIData.dat"
# setOutput(muonFakeRoI, "OutputDecisions", "MuonRoIs")
# setOutput(muonFakeRoI, "OutputRoIs", "MuonTrigRoIs")
# setOutput(muonFakeRoI, "OutputRecEMTauRoIs", "MuonRecRoIs")
# muonFakeRoI.OutputLevel=DEBUG
# topSequence += muonFakeRoI

# from L1Decoder.L1DecoderConf import FakeCTP
# fakeCTP = FakeCTP()
# fakeCTP.OutputLevel=VERBOSE
# fakeCTP.InputFilename="menuData.dat"
# fakeCTP.OutputLevel=VERBOSE
# topSequence += fakeCTP

# from ViewAlgs.ViewAlgsConf import PrescaleDecision
# ps = PrescaleDecision("HLTChainsPrescaling")
# connectAlgorithmsIO(consumer=(ps, "InputChainDecisions"), producer=(fakeCTP, "OutputDecisions"))
# #ps.InputChainDecisions = getOutput(fakeCTP, "OutputDecisions")
# setOutput(ps, "OutputChainDecisions", "ChainsPassingAfterPS")
# ps.OutputLevel=DEBUG
# topSequence += ps

# from ViewAlgs.ViewAlgsConf import DecisionAlg, CopyPassing
# activeRoIsAfterPrescaling = genDecisionAlg("menuStep0")
# connectAlgorithmsIO(consumer=(activeRoIsAfterPrescaling, "InputChainDecisions"), producer=(ps, "OutputChainDecisions"))
# connectHypoToMenu(consumer=activeRoIsAfterPrescaling, producer=(caloFakeRoI, "OutputDecisions"))
# connectHypoToMenu(consumer=activeRoIsAfterPrescaling, producer=(muonFakeRoI, "OutputDecisions"))
# topSequence += activeRoIsAfterPrescaling

# caloRoIsAfterPrescaling = CopyPassing("copyCaloRoIsPassingPrecaling")
# connectAlgorithmsIO( consumer=(caloRoIsAfterPrescaling, "CollectionBeforeMenu"), producer=(caloFakeRoI, "OutputDecisions"))
# connectAlgorithmsIO( consumer=(caloRoIsAfterPrescaling, "CollectionAfterMenu"),  producer=(activeRoIsAfterPrescaling, "OutputHypoDecisions") )
# setOutput(caloRoIsAfterPrescaling, "Output", "CaloRoIsAfterPrescaling" )
# caloRoIsAfterPrescaling.OutputLevel=DEBUG
# topSequence +=  caloRoIsAfterPrescaling

# #caloRoIsAfterPrescaling = activeRoIsAfterPrescaling
# muonRoIsAfterPrescaling = CopyPassing("copyMuonRoIsPassingPrecaling")
# connectAlgorithmsIO( consumer=(muonRoIsAfterPrescaling, "CollectionBeforeMenu"), producer=(muonFakeRoI, "OutputDecisions"))
# connectAlgorithmsIO( consumer=(muonRoIsAfterPrescaling, "CollectionAfterMenu"),  producer=(activeRoIsAfterPrescaling, "OutputHypoDecisions") )
# setOutput(muonRoIsAfterPrescaling, "Output", "MuonRoIsAfterPrescaling" )
# muonRoIsAfterPrescaling.OutputLevel=DEBUG
# topSequence +=  muonRoIsAfterPrescaling
# #muonRoIsAfterPrescaling = activeRoIsAfterPrescaling



# # caloRoIsAfterPrescaling = genDecisionAlg("caloRoIsAfterPrescaling")
# # connectAlgorithmsIO(consumer=(caloRoIsAfterPrescaling, "InputChainDecisions"), producer=(ps, "OutputChainDecisions"))
# # connectHypoToMenu(consumer=caloRoIsAfterPrescaling, producer=(caloFakeRoI, "OutputDecisions"))
# # topSequence += caloRoIsAfterPrescaling


# # muonRoIsAfterPrescaling = genDecisionAlg("muonRoIsAfterPrescaling")
# # caloRoIsAfterPrescaling
# # connectAlgorithmsIO(consumer=(muonRoIsAfterPrescaling, "InputChainDecisions"), producer=(ps, "OutputChainDecisions"))
# # connectHypoToMenu(consumer=muonRoIsAfterPrescaling, producer=(muonFakeRoI, "OutputDecisions"))
# # topSequence += muonRoIsAfterPrescaling

# # Start L2

# # calo L2 algorithm ------------
# from ViewAlgsTest.ViewAlgsTestConf import TestFEXAlg
# l2CaloReco = TestFEXAlg("L2CaloReco")
# connectAlgorithmsIO(consumer=(l2CaloReco, "RoIsContainer"), producer=(caloRoIsAfterPrescaling, "Output"))
# #l2CaloReco.RoIsContainer = getOutput(dropPrescaled, "OutputDecisions")
# setOutput(l2CaloReco, "OutputClusterContainer", "L2CaloFex")
# setOutput(l2CaloReco, "OutputProxyContainer", "L2CaloFexProxy")
# l2CaloReco.OutputLevel=DEBUG
# topSequence += l2CaloReco

# from ViewAlgsTest.ViewAlgsTestConf import TestHypoAlg, TestHypoTool
# l2CaloHypo = TestHypoAlg("L2CaloHypo")
# l2CaloHypo.OutputLevel=DEBUG
# connectAlgorithmsIO(consumer=(l2CaloHypo, "InputProxyContainer"), producer=(l2CaloReco, "OutputProxyContainer"))
# #l2CaloHypo.InputProxyContainer = getOutput(l2CaloReco,"OutputProxyContainer")
# setOutput(l2CaloHypo, "OutputDecisions",l2CaloHypo.name()+"Decisions")

# l2cut3 = TestHypoTool("L2_3GeVCaloEtCut", EtThreshold=3.3)
# l2cut7 = TestHypoTool("L2_7GeVCaloEtCut", EtThreshold=7.2)
# l2cut10 = TestHypoTool("L2_10GeVCaloEtCut", EtThreshold=10.4)
# l2cut12 = TestHypoTool("L2_12GeVCaloEtCut", EtThreshold=12.2)
# # cuts 
# l2CaloHypo += l2cut3
# l2CaloHypo += l2cut7
# l2CaloHypo += l2cut10
# l2CaloHypo += l2cut12
# l2CaloHypo.Hypotheses = [l2cut3, l2cut7, l2cut10, l2cut12]
# topSequence += l2CaloHypo


# from ViewAlgs.ViewAlgsConf import DecisionAlg

# l2CaloStep = genDecisionAlg("MenuL2CaloStep") #, [l2CaloHypo], [caloRoIsAfterPrescaling])
# connectAlgorithmsIO(consumer=(l2CaloStep, "InputChainDecisions"), producer=(caloRoIsAfterPrescaling, "Output"))
# connectHypoToMenu(consumer=l2CaloStep, producer=(l2CaloHypo, "OutputDecisions"))
# l2CaloStep.OutputLevel=VERBOSE
# topSequence += l2CaloStep

# #Dumper
# from ViewAlgs.ViewAlgsConf import DumpDecisions
# l2Dumper = DumpDecisions("L2CaloRoIsDumper")
# l2Dumper.OutputLevel=VERBOSE
# connectAlgorithmsIO(consumer=(l2Dumper, "OutputDecisions"), producer=(l2CaloHypo, "OutputDecisions"))
# #l2Dumper.OutputDecisions=getOutput(l2CaloHypo, "OutputDecisions")
# topSequence += l2Dumper



# # muon L2 Algorithm -------------------------
# l2MuonReco = TestFEXAlg("L2MuonReco")
# connectAlgorithmsIO(consumer=(l2MuonReco, "RoIsContainer"), producer=(muonRoIsAfterPrescaling, "Output"))
# #l2MuonReco.RoIsContainer = getOutput(dropPrescaled, "OutputDecisions")
# setOutput(l2MuonReco, "OutputClusterContainer", "L2MuonFex")
# setOutput(l2MuonReco, "OutputProxyContainer", "L2MuonFexProxy")
# l2MuonReco.OutputLevel=DEBUG
# topSequence += l2MuonReco


# l2MuonHypo = TestHypoAlg("L2MuonHypo")
# l2MuonHypo.OutputLevel=DEBUG
# connectAlgorithmsIO(consumer=(l2MuonHypo, "InputProxyContainer"), producer=(l2MuonReco, "OutputProxyContainer"))
# #l2MuonHypo.InputProxyContainer = getOutput(l2MuonReco,"OutputProxyContainer")
# setOutput(l2MuonHypo, "OutputDecisions",l2MuonHypo.name()+"Decisions")

# l2cut6mu = TestHypoTool("L2_6GeVMuonCut", EtThreshold=6.0)

# # cuts 
# l2MuonHypo += l2cut6mu
# l2MuonHypo.Hypotheses = [l2cut6mu]
# topSequence += l2MuonHypo

# l2MuonStep = genDecisionAlg("MenuL2MuonStep") #, [l2MuonHypo], [muonRoIsAfterPrescaling])
# connectAlgorithmsIO(consumer=(l2MuonStep, "InputChainDecisions"), producer=(activeRoIsAfterPrescaling, "OutputChainDecisions"))
# connectHypoToMenu(consumer=l2MuonStep, producer=(l2MuonHypo, "OutputDecisions"))
# l2MuonStep.OutputLevel=VERBOSE
# topSequence += l2MuonStep


# l2ComboStep = genDecisionAlg("MenuStepL2Combo")
# connectAlgorithmsIO(consumer=(l2ComboStep, "InputChainDecisions"), producer=(activeRoIsAfterPrescaling, "OutputChainDecisions"))
# connectAlgorithmsIO(consumer=(l2ComboStep, "InputChainDecisions"), producer=(activeRoIsAfterPrescaling, "OutputChainDecisions"))
# connectHypoToMenu(consumer=l2ComboStep, producer=(l2MuonHypo, "OutputDecisions"))
# connectHypoToMenu(consumer=l2ComboStep, producer=(l2CaloHypo, "OutputDecisions"))
# topSequence += l2ComboStep

# # next step - better calo
# efCaloReco = TestFEXAlg("EFCaloReco")
# # without merging:
# connectAlgorithmsIO(consumer=(efCaloReco, "RoIsContainer"), producer=(l2CaloStep, "OutputHypoDecisions"))
# #efCaloReco.RoIsContainer = getOutput(l2CaloStep, "OutputDecisions")
# #efCaloReco.RoIsContainer = getOutput(mergeAlgo, "MergedProxyContainer")
# setOutput(efCaloReco, "OutputClusterContainer", "EFCaloFex")
# setOutput(efCaloReco, "OutputProxyContainer", "EFCaloFexProxy")
# efCaloReco.OutputLevel=DEBUG
# topSequence += efCaloReco


# efCaloHypo = TestHypoAlg("EFCaloHypo")
# efCaloHypo.OutputLevel=DEBUG
# connectAlgorithmsIO(consumer=(efCaloHypo, "InputProxyContainer"), producer=(efCaloReco, "OutputProxyContainer"))
# #efCaloHypo.InputProxyContainer = getOutput(efCaloReco,"OutputProxyContainer")
# setOutput(efCaloHypo, "OutputDecisions",efCaloHypo.name()+"Decisions")

# # cuts 
# efcut3 = TestHypoTool("EF_3GeVCaloEtCut", EtThreshold=3.3)
# efcut7 = TestHypoTool("EF_7GeVCaloEtCut", EtThreshold=7.2)
# efcut10 = TestHypoTool("EF_10GeVCaloEtCut", EtThreshold=10.4)
# efcut12 = TestHypoTool("EF_12GeVCaloEtCut", EtThreshold=12.2)
# efCaloHypo += efcut3
# efCaloHypo += efcut7
# efCaloHypo += efcut10
# efCaloHypo += efcut12
# efCaloHypo.Hypotheses = [efcut3, efcut7, efcut10, efcut12]
# topSequence += efCaloHypo


# efCaloStep = genDecisionAlg("MenuEFCaloStep")#, [efCaloHypo], [l2CaloStep])
# connectAlgorithmsIO(consumer=(efCaloStep, "InputChainDecisions"), producer=(l2CaloStep, "OutputChainDecisions"))
# connectHypoToMenu(consumer=efCaloStep, producer=(efCaloHypo, "OutputDecisions"))
# efCaloStep.OutputLevel=VERBOSE
# topSequence += efCaloStep

# from ViewAlgs.ViewAlgsConf import DumpDecisions
# efDumper = DumpDecisions("EFCaloRoIsDumper")
# efDumper.OutputLevel=VERBOSE
# connectAlgorithmsIO(consumer=(efDumper, "OutputDecisions"), producer=(efCaloHypo, "OutputDecisions"))
# topSequence += efDumper


# # here we can have fake tracking
# # we ought to import merge RoIs alg which is fed by 
# # MuonRoIs produced by the muonFakeRoIs 
# # CaloRoIs produced by upstream alg l2CaloHypo (reduced set for a change)
# # result of it is merged/super RoIs set
# #
# # then we run tracking 
# #
# # and RoI splitter 

# ##--------------------------
# # test merging/splitting
# ##--------------------------
# #  merge of Calo RoIs
# from ViewAlgsTest.ViewAlgsTestConf import TestMergeAlg
# mergeAlgo = TestMergeAlg("MergeAllRoIs")
# connectAlgorithmsIO(consumer=(mergeAlgo, "InpuCollections"), producer=(l2CaloStep, "OutputHypoContainer"), propset=False)
# connectAlgorithmsIO(consumer=(mergeAlgo, "InpuCollections"), producer=(l2MuonStep, "OutputHypoContainer"), propset=False)
# setOutput(mergeAlgo, "MergedProxyContainer", "MergedRoIs")
# mergeAlgo.OutputLevel=DEBUG
# topSequence += mergeAlgo

# # next step - add here tracking on the merged Container (one RoI)
# # fake tracking now
# MergedTrackReco = TestFEXAlg("MergedTracking")
# connectAlgorithmsIO(consumer=(MergedTrackReco, "RoIsContainer"), producer=(mergeAlgo, "MergedProxyContainer"))
# #MergedTrackReco.RoIsContainer = getOutput(mergeAlgo, "MergedProxyContainer")
# setOutput(MergedTrackReco, "OutputClusterContainer", "MergedTrackCollection") 
# setOutput(MergedTrackReco, "OutputProxyContainer", "MergedTrackFexProxy")
# MergedTrackReco.OutputLevel=DEBUG
# topSequence += MergedTrackReco

# # then splitting again....
# # retrieve Calo RoIs
# from ViewAlgsTest.ViewAlgsTestConf import TestSplitAlg
# splitCaloAlgo = TestSplitAlg("SplitCaloRoIs")
# connectAlgorithmsIO(consumer=(splitCaloAlgo, "InputProxyContainer"), producer=(MergedTrackReco, "OutputProxyContainer"))
# #splitCaloAlgo.InputProxyContainer = getOutput(MergedTrackReco, "OutputProxyContainer")
# connectAlgorithmsIO(consumer=(splitCaloAlgo, "ReferenceProxyContainer"), producer=(l2CaloStep, "OutputHypoDecisions"))
# #splitCaloAlgo.ReferenceProxyContainer = getOutput(l2CaloStep, "OutputDecisions")
# setOutput(splitCaloAlgo, "SplitProxyContainer", "SplitCaloRoIs")
# splitCaloAlgo.OutputLevel=DEBUG
# topSequence += splitCaloAlgo

# # retrieve Muon RoIs
# splitMuonAlgo = TestSplitAlg("SplitMuonRoIs")
# connectAlgorithmsIO(consumer=(splitMuonAlgo, "InputProxyContainer"), producer=(MergedTrackReco, "OutputProxyContainer"))
# #splitMuonAlgo.InputProxyContainer = getOutput(MergedTrackReco, "OutputProxyContainer")
# connectAlgorithmsIO(consumer=(splitMuonAlgo, "ReferenceProxyContainer"), producer=(l2MuonStep, "OutputHypoDecisions"))
# #splitMuonAlgo.ReferenceProxyContainer = getOutput(l2MuonStep, "OutputDecisions")
# setOutput(splitMuonAlgo, "SplitProxyContainer", "SplitMuonRoIs")
# splitMuonAlgo.OutputLevel=DEBUG
# topSequence += splitMuonAlgo

# # do track-cluster match
# CaloMatchingFex = TestFEXAlg("CaloMatchingFex")
# connectAlgorithmsIO(consumer=(CaloMatchingFex, "RoIsContainer"), producer=(splitCaloAlgo, "SplitProxyContainer"))
# #CaloMatchingFex.RoIsContainer = getOutput(splitCaloAlgo, "SplitProxyContainer")
# # need to add link to track collection
# setOutput(CaloMatchingFex, "OutputClusterContainer", "CaloMatchingCollection") 
# setOutput(CaloMatchingFex, "OutputProxyContainer", "CaloMatchingFexProxy")
# CaloMatchingFex.OutputLevel=DEBUG
# topSequence += CaloMatchingFex

# # calo-mathcing hypo
# CaloMatchingHypo = TestHypoAlg("CaloMatchingHypo")
# CaloMatchingHypo.OutputLevel=DEBUG
# connectAlgorithmsIO(consumer=(CaloMatchingHypo, "InputProxyContainer"), producer=(CaloMatchingFex, "OutputProxyContainer"))
# #CaloMatchingHypo.InputProxyContainer = getOutput(CaloMatchingFex,"OutputProxyContainer")
# setOutput(CaloMatchingHypo, "OutputDecisions",CaloMatchingHypo.name()+"Decisions")

# # cuts 
# CaloMatchingHypo += efcut3
# CaloMatchingHypo += efcut7
# CaloMatchingHypo += efcut10
# CaloMatchingHypo += efcut12
# CaloMatchingHypo.Hypotheses = [efcut3, efcut7, efcut10, efcut12]
# topSequence += CaloMatchingHypo


# # menu Algo for calo-tracking match
# caloTrackingStep = genDecisionAlg("MenuCaloTrackingStep")#, [CaloMatchingHypo], [l2CaloStep]) 
# connectAlgorithmsIO(consumer=(caloTrackingStep, "InputChainDecisions"), producer=(efCaloStep, "OutputChainDecisions"))
# connectHypoToMenu(consumer=caloTrackingStep, producer=(CaloMatchingHypo, "OutputDecisions"))
# caloTrackingStep.OutputLevel=VERBOSE
# topSequence += caloTrackingStep


# CaloTrackingDumper = DumpDecisions("CaloTrackingDumper")
# CaloTrackingDumper.OutputLevel=VERBOSE
# connectAlgorithmsIO(consumer=(CaloTrackingDumper, "OutputDecisions"), producer=(CaloMatchingHypo, "OutputDecisions"))
# #CaloTrackingDumper.OutputDecisions=getOutput(CaloMatchingHypo, "OutputDecisions")
# topSequence += CaloTrackingDumper






# ##--------------------------
# # Make the chains
# ##--------------------------

# # chains = ChainsList()
# # chains + Chain("HLT_e3", "L1_EM3", 20.99, {"caloRoIsAfterPrescaling": "EM3 x 1", 
# #                                            "MenuL2CaloStep": "L2_3GeVCaloEtCut x 1", 
# #                                            "MenuEFCaloStep": "EF_3GeVCaloEtCut x 1"}) 
# # chains + Chain("HLT_2e3", "L1_2EM3", 1, {"caloRoIsAfterPrescaling": "EM3 x 2", 
# #                                           "MenuL2CaloStep": "L2_3GeVCaloEtCut x 2",
# #                                           "MenuEFCaloStep": "EF_3GeVCaloEtCut x 2"})

# # chains + Chain("HLT_e3e7", "L1_2EM3", 1, {"caloRoIsAfterPrescaling": "EM3 x 1, EM7 x 1", 
# #                                           "MenuL2CaloStep": "L2_3GeVCaloEtCut x 1, L2_7GeVCaloEtCut x 1",
# #                                           "MenuEFCaloStep": "EF_3GeVCaloEtCut x 1, EF_7GeVCaloEtCut x 1"})

# # chains + Chain("HLT_e7", "L1_EM7", 1, {"caloRoIsAfterPrescaling": "EM7 x 1", 
# #                                         "MenuL2CaloStep": "L2_7GeVCaloEtCut x 1",
# #                                         "MenuEFCaloStep": "EF_7GeVCaloEtCut x 1"})
# # chains + Chain("HLT_e10", "L1_EM10", 1, {"caloRoIsAfterPrescaling": "EM10 x 1", 
# #                                           "MenuL2CaloStep": "L2_10GeVCaloEtCut x 1",
# #                                           "MenuEFCaloStep": "EF_10GeVCaloEtCut x 1"})
# # chains + Chain("HLT_mu6", "L1_MU6", 1, {"muonRoIsAfterPrescaling": "MU6 x 1", 
# #                                           "MenuL2MuonStep": "L2_6GeVMuonCut x 1"})

# # chains + Chain("HLT_mu6e3", "L1_EM3MU6", 1, {"caloRoIsAfterPrescaling": "EM3 x 1", 
# #                                              "muonRoIsAfterPrescaling": "MU6 x 1",
# #                                              "MenuStepL2Combo": "L2_3GeVCaloEtCut x 1, L2_6GeVMuonCut x 1"} )

# # chains + Chain("HLT_e3merged", "L1_EM3", 20.99, {"caloRoIsAfterPrescaling": "EM3 x 1", 
# #                                            "MenuL2CaloStep": "L2_3GeVCaloEtCut x 1", 
# #                                            "MenuCaloTrackingStep": "EF_3GeVCaloEtCut x 1"}) 

# # chains + Chain("HLT_e7merged", "L1_EM7", 1, {"caloRoIsAfterPrescaling": "EM7 x 1", 
# #                                         "MenuL2CaloStep": "L2_7GeVCaloEtCut x 1",
# #                                         "MenuCaloTrackingStep": "EF_7GeVCaloEtCut x 1"})

# # chains + Chain("HLT_e7mu6_merged", "L1_EM7,L1_MU6", 1, {"caloRoIsAfterPrescaling": "EM7 x 1, MU6 x 1", 
# #                                         "MenuL2CaloStep": "L2_7GeVCaloEtCut x 1",
# #                                         "MenuL2MuonStep": "L2_6GeVMuonCut x 1",
# #                                         "MenuCaloTrackingStep": "EF_7GeVCaloEtCut x 1"})
 

# # configureMenu(chains, topSequence)

# # this is not yet working allright
# from ViewAlgsTest.Menu import menu_ingredients
# configureFromXML( "HLTTestMenu.xml", menu_ingredients, topSequence)


# #--------------------------------------------------------------
# # Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
# #--------------------------------------------------------------
# svcMgr.MessageSvc.OutputLevel = DEBUG
# svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
# svcMgr.MessageSvc.verboseLimit = 0
# svcMgr.StoreGateSvc.OutputLevel = INFO
# svcMgr.StoreGateSvc.Dump=True
# #print svcMgr
# MessageSvc.debugLimit=1000000
theApp.EvtMax = 4
# theApp.SkipEvents=3

# print "topSequence dump:", topSequence
# #
# # End of job options file
# #
# ###############################################################

# graph_generator.saveGraph('menuRun dependencies graph', 'graph.dot')
