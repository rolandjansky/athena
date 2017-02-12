#
# get_files -xmls LVL1config_Physics_pp_v5.xml(or acquire it by some other means )
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

StatusCodeSvc.AbortOnError=False



## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence( )


#define way of running with or without Gaudi sequneces:
runGaudiSequence=False
if runGaudiSequence:
    from AthenaCommon.AlgSequence import AthSequencer
    job += AthSequencer("MySeq" )
    topSequence = job.MySeq
else:
    topSequence = job



#topSequence = AlgSequence( )




from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc" )
l1svc.XMLMenuFile = "LVL1config_Physics_pp_v5.xml"
svcMgr += l1svc


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from ViewAlgsTest.Menu import *
from ViewAlgsTest.connectAlgorithmsIO import *

#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
caloFakeRoI = FakeRoI( "caloFakeRoI" )
caloFakeRoI.InputFilename="caloRoIData.dat"
setOutput( caloFakeRoI, "OutputDecisions", "CaloRoIs" )
setOutput( caloFakeRoI, "OutputRoIs", "CaloTrigRoIs" )
setOutput( caloFakeRoI, "OutputRecEMTauRoIs", "CaloRecRoIs" )


# from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
# setTHistSvcOutput([] )
# if not hasattr(ServiceMgr, 'THistSvc' ):       
#     from GaudiSvc.GaudiSvcConf import THistSvc
#     ServiceMgr += THistSvc( )
#     if hasattr(ServiceMgr.THistSvc, "Output" ): # this is offline THistSvc fo which we want to setup files
#         setTHistSvcOutput(ServiceMgr.THistSvc.Output )

# from TrigMonitorBase.TrigGenericMonitoringToolConfig import *
# mt = TrigGenericMonitoringTool("CaloRoIsMonitor" )
# mt.OutputLevel=VERBOSE
# mt.Histograms += [defineHistogram("roiCount", path="EXPERT", type="TH1F", title="RoIs count;#RoIs;events", xbins=30, xmin=0, xmax=30 )]
# caloFakeRoI.monTools += [mt]



caloFakeRoI.OutputLevel=DEBUG
topSequence += caloFakeRoI

# Test the execution of an algorithm within an event view
testViewAlgorithm = False
if( testViewAlgorithm  ):

  # The algorithm to run inside the views
  from ViewAlgsTest.ViewAlgsTestConf import SchedulerProxyAlg
  viewAlgName = "algInView"
  viewAlg = SchedulerProxyAlg( viewAlgName  )
  viewAlg.RequireView = True
  topSequence += viewAlg

  # The pool to retrieve the view algorithm from
  from GaudiHive.GaudiHiveConf import AlgResourcePool
  viewAlgPoolName = "ViewAlgPool"
  svcMgr += AlgResourcePool( viewAlgPoolName  )
  svcMgr.ViewAlgPool.TopAlg = [ viewAlgName ]

  # The algorithm to launch the views
  from ViewAlgsTest.ViewAlgsTestConf import TestViewDriver
  runInViews = TestViewDriver( "runInViews"  )
  connectAlgorithmsIO( producer=(caloFakeRoI, "OutputRoIs" ), consumer=(runInViews, "RoIsContainer" ) ) 
  runInViews.OutputLevel = DEBUG
  runInViews.ViewAlgorithmNames = [ viewAlgName ]
  topSequence += runInViews


muonFakeRoI = FakeRoI( "muonFakeRoI" )
muonFakeRoI.InputFilename="muonRoIData.dat"
setOutput( muonFakeRoI, "OutputDecisions", "MuonRoIs" )
setOutput( muonFakeRoI, "OutputRoIs", "MuonTrigRoIs" )
setOutput( muonFakeRoI, "OutputRecEMTauRoIs", "MuonRecRoIs" )
muonFakeRoI.OutputLevel=DEBUG
topSequence += muonFakeRoI

from L1Decoder.L1DecoderConf import FakeCTP
fakeCTP = FakeCTP(  )
fakeCTP.OutputLevel=VERBOSE
fakeCTP.InputFilename="menuData.dat"
fakeCTP.OutputLevel=VERBOSE
topSequence += fakeCTP

from ViewAlgs.ViewAlgsConf import PrescaleDecision
ps = PrescaleDecision( "HLTChainsPrescaling" )
#ps.InputChainDecisions = getOutput( fakeCTP, "OutputDecisions" )
setOutput( ps, "OutputChainDecisions", "ChainsPassingAfterPS" )
ps.OutputLevel=DEBUG
topSequence += ps

from ViewAlgs.ViewAlgsConf import DecisionAlg, CopyPassing

# Start L2
menuStep0 = genDecisionAlg( "menuStep0" ) # this is event wide desision
connectHypoToMenu( consumer=menuStep0, producer=( caloFakeRoI, "OutputDecisions" ) )
connectHypoToMenu( consumer=menuStep0, producer=( muonFakeRoI, "OutputDecisions" ) )
topSequence += menuStep0

caloRoIsAfterPrescaling = CopyPassing( "copyCaloRoIsPassingPrecaling" )
setOutput( caloRoIsAfterPrescaling, "Output", "CaloRoIsAfterPrescaling"  )
caloRoIsAfterPrescaling.OutputLevel=DEBUG
topSequence +=  caloRoIsAfterPrescaling


muonRoIsAfterPrescaling = CopyPassing( "copyMuonRoIsPassingPrecaling" )
setOutput( muonRoIsAfterPrescaling, "Output", "MuonRoIsAfterPrescaling"  )
muonRoIsAfterPrescaling.OutputLevel=DEBUG
topSequence +=  muonRoIsAfterPrescaling

# Calo L2 algorithm ------------
from ViewAlgsTest.ViewAlgsTestConf import TestFEXAlg
l2CaloReco = TestFEXAlg( "L2CaloReco" )
#l2CaloReco.RoIsContainer = getOutput( dropPrescaled, "OutputDecisions" )
setOutput( l2CaloReco, "OutputClusterContainer", "L2CaloFex" )
setOutput( l2CaloReco, "OutputProxyContainer", "L2CaloFexProxy" )
l2CaloReco.OutputLevel=DEBUG
topSequence += l2CaloReco

from ViewAlgsTest.ViewAlgsTestConf import TestHypoAlg, TestHypoTool
l2CaloHypo = TestHypoAlg( "L2CaloHypo" )
l2CaloHypo.OutputLevel=DEBUG
setOutput( l2CaloHypo, "OutputDecisions",l2CaloHypo.name(  )+"Decisions" )

l2cut3 = TestHypoTool( "L2_3GeVCaloEtCut", EtThreshold=3.3 )
l2cut7 = TestHypoTool( "L2_7GeVCaloEtCut", EtThreshold=7.2 )
l2cut10 = TestHypoTool( "L2_10GeVCaloEtCut", EtThreshold=10.4 )
l2cut12 = TestHypoTool( "L2_12GeVCaloEtCut", EtThreshold=12.2 )
# cuts 
l2CaloHypo += l2cut3
l2CaloHypo += l2cut7
l2CaloHypo += l2cut10
l2CaloHypo += l2cut12
l2CaloHypo.Hypotheses = [l2cut3, l2cut7, l2cut10, l2cut12]
topSequence += l2CaloHypo


from ViewAlgs.ViewAlgsConf import DecisionAlg

menuStep1 = genDecisionAlg( "menuStep1" )
connectHypoToMenu( consumer=menuStep1, producer=( l2CaloHypo, "OutputDecisions" ) )
menuStep1.OutputLevel=VERBOSE
topSequence += menuStep1

#Dumper
from ViewAlgs.ViewAlgsConf import DumpDecisions
l2Dumper = DumpDecisions( "L2CaloRoIsDumper" )
l2Dumper.OutputLevel=VERBOSE
topSequence += l2Dumper



# muon L2 Algorithm -------------------------
l2MuonReco = TestFEXAlg( "L2MuonReco" )
setOutput( l2MuonReco, "OutputClusterContainer", "L2MuonFex" )
setOutput( l2MuonReco, "OutputProxyContainer", "L2MuonFexProxy" )
l2MuonReco.OutputLevel=DEBUG
topSequence += l2MuonReco


l2MuonHypo = TestHypoAlg( "L2MuonHypo" )
l2MuonHypo.OutputLevel=DEBUG
setOutput( l2MuonHypo, "OutputDecisions",l2MuonHypo.name(  )+"Decisions" )
l2cut6mu = TestHypoTool( "L2_6GeVMuonCut", EtThreshold=6.0 )
# cuts 
l2MuonHypo += l2cut6mu
l2MuonHypo.Hypotheses = [l2cut6mu]
topSequence += l2MuonHypo


# get set of reduced RoIs after menu step 1
caloRoIsAfterStep1 = CopyPassing( "copyCaloRoIsPassingStep1" )
setOutput( caloRoIsAfterStep1, "Output", "CaloRoIsAfterStep1"  )
caloRoIsAfterStep1.OutputLevel=DEBUG
topSequence +=  caloRoIsAfterStep1


muonRoIsAfterStep1 = CopyPassing( "copyMuonRoIsPassingStep1" )
setOutput( muonRoIsAfterStep1, "Output", "MuonRoIsAfterStep1"  )
muonRoIsAfterStep1.OutputLevel=DEBUG
topSequence +=  muonRoIsAfterStep1

from ViewAlgsTest.ViewAlgsTestConf import MergeRoIsAlg
roisForTracking = MergeRoIsAlg( "RoISForTracking" )
setOutput( roisForTracking, "OutputRoIContainer", "RoISForTracking"  )
roisForTracking.OutputLevel=DEBUG
topSequence +=  roisForTracking

# fake tracking now

MergedTrackReco = TestFEXAlg( "MergedTracking" )
setOutput( MergedTrackReco, "OutputClusterContainer", "MergedTrackCollection" ) 
setOutput( MergedTrackReco, "OutputProxyContainer", "MergedTrackFexProxy" )
MergedTrackReco.OutputLevel=DEBUG
topSequence += MergedTrackReco


from ViewAlgsTest.ViewAlgsTestConf import TestSplitAlg
splitCaloAlgo = TestSplitAlg( "SplitCaloRoIs" )
setOutput( splitCaloAlgo, "SplitProxyContainer", "SplitCaloRoIs" )
splitCaloAlgo.OutputLevel=DEBUG
topSequence += splitCaloAlgo

# retrieve Muon RoIs
splitMuonAlgo = TestSplitAlg( "SplitMuonRoIs" )
connectAlgorithmsIO( consumer=( splitMuonAlgo, "InputProxyContainer" ), producer=( MergedTrackReco, "OutputProxyContainer" ) )
#splitMuonAlgo.InputProxyContainer = getOutput( MergedTrackReco, "OutputProxyContainer" )

#splitMuonAlgo.ReferenceProxyContainer = getOutput( l2MuonStep, "OutputDecisions" )
setOutput( splitMuonAlgo, "SplitProxyContainer", "SplitMuonRoIs" )
splitMuonAlgo.OutputLevel=DEBUG
topSequence += splitMuonAlgo


# do track-cluster match
from ViewAlgsTest.ViewAlgsTestConf import TestCombiner
CaloMatchingFex = TestCombiner( "CaloMatchingFex" )
#setOutput( CaloMatchingFex, "Output", "Electrons" ) 
CaloMatchingFex.OutputLevel=DEBUG
topSequence += CaloMatchingFex

# calo-mathcing hypo
l2EHypo = TestHypoAlg( "L2_eHypo" )
l2EHypo.OutputLevel=DEBUG
setOutput( l2EHypo, "OutputDecisions", "L2ElectronDecisions" )

# cuts 
ecut3  = TestHypoTool( "L2_3GeVCaloEtCut", EtThreshold=3.3 )
ecut7  = TestHypoTool( "L2_7GeVCaloEtCut", EtThreshold=7.2 )
ecut10 = TestHypoTool( "L2_10GeVCaloEtCut", EtThreshold=10.4 )
ecut12 = TestHypoTool( "L2_12GeVCaloEtCut", EtThreshold=12.2 )
l2EHypo += ecut3
l2EHypo += ecut7
l2EHypo += ecut10
l2EHypo += ecut12
l2EHypo.Hypotheses = [ecut3, ecut7, ecut10, ecut12]
topSequence += l2EHypo



# do track-cluster match
MuonMatchingFex = TestCombiner( "CaloMatchingFex" )
#setOutput( CaloMatchingFex, "Output", "Electrons" ) 
CaloMatchingFex.OutputLevel=DEBUG
topSequence += CaloMatchingFex



menuStep2 = genDecisionAlg( "menuStep2" )
topSequence += menuStep2

menuStep3 = genDecisionAlg( "menuStep3" )
topSequence += menuStep3







print "-"*80
print "-- constructing workflow"
print "-"*80
print ""
# the << and >> are absolutely symmetric, except that lists/tuples can only be placed on the right hand side of this operators
# I decided to use the convention that the setup looks menu crentric (i.e. menu algs are destinations and sources, i.e. menuStep* is on the left side)
#

# chains seeding and prescaling
DH( ps, "InputChainDecisions" )        << DH( fakeCTP, "OutputDecisions" )
DH( menuStep0, "InputChainDecisions" ) << DH( ps, "OutputChainDecisions" ) 
DH( menuStep0, "InputHypoDecisions" )  << [ DH( caloFakeRoI, "OutputDecisions" ), 
                                             DH( muonFakeRoI, "OutputDecisions" ) ]

print ""
# provision of filtered RoIs
DH( caloFakeRoI, "OutputDecisions" )   >> DH( caloRoIsAfterPrescaling, "CollectionBeforeMenu" )
DH( muonFakeRoI, "OutputDecisions" )   >> DH( muonRoIsAfterPrescaling, "CollectionBeforeMenu" ) 
DH( menuStep0, "OutputHypoDecisions" ) >> [ DH( muonRoIsAfterPrescaling, "CollectionAfterMenu" ),
                                            DH( caloRoIsAfterPrescaling, "CollectionAfterMenu" ) ]

print ""
# L2 calo reco and hypo
DH( caloRoIsAfterPrescaling, "Output" )  >> DH( l2CaloReco, "RoIsContainer" )
DH( l2CaloReco, "OutputProxyContainer" ) >> DH( l2CaloHypo, "InputProxyContainer" )
#DH( l2CaloHypo, "OutputDecisions" )      >> DH( l2Dumper, "OutputDecisions" ) # printing

print ""
# L2 muon and hypo
DH( muonRoIsAfterPrescaling, "Output" )  >> DH( l2MuonReco, "RoIsContainer" )
DH( l2MuonReco, "OutputProxyContainer" ) >> DH( l2MuonHypo, "InputProxyContainer" )

print ""
# second menu step 
DH( menuStep1, "InputChainDecisions" )  << DH( menuStep0, "OutputChainDecisions" )  

DH( menuStep1, "InputHypoDecisions")    << [ DH(l2CaloHypo, "OutputDecisions"), 
                                             DH(l2MuonHypo, "OutputDecisions") ]

print ""
# common tracking
# provision of filtered RoIs
DH( l2CaloHypo, "OutputDecisions" )   >> DH( caloRoIsAfterStep1, "CollectionBeforeMenu" )
DH( l2MuonHypo, "OutputDecisions" )   >> DH( muonRoIsAfterStep1, "CollectionBeforeMenu" ) 
DH( menuStep1, "OutputHypoDecisions" ) >> [ DH( muonRoIsAfterStep1, "CollectionAfterMenu" ),
                                            DH( caloRoIsAfterStep1, "CollectionAfterMenu" ) ]


# merged RoIs
DH( roisForTracking, "InputRoIContainers") << [ DH(caloRoIsAfterStep1, "Output"), 
                                                DH(muonRoIsAfterStep1, "Output")]
# tracking 
DH( roisForTracking, "OutputRoIContainer") >> DH( MergedTrackReco, "RoIsContainer" )


# split tracks back
DH( l2CaloHypo, "OutputDecisions" ) >> DH( splitCaloAlgo, "ReferenceProxyContainer" )
DH( l2MuonHypo, "OutputDecisions" ) >> DH( splitMuonAlgo, "ReferenceProxyContainer" )

DH( MergedTrackReco, "OutputProxyContainer" ) >> [ DH( splitCaloAlgo, "InputProxyContainer" ),
                                                   DH( splitMuonAlgo, "InputProxyContainer" ) ]


DH(splitCaloAlgo, ""  )

DH( menuStep2, "InputChainDecisions" ) << DH( menuStep1, "OutputChainDecisions" )
DH( menuStep3, "InputChainDecisions" ) << DH( menuStep2, "OutputChainDecisions" )


graph_generator.saveGraph("workflow", "workflow.dot")


print ""
print "-"*80
print "-- constructing workflow done"
print "-"*80
kaaaaboom # want to stop the job now







# # next step - better calo
# efCaloReco = TestFEXAlg( "EFCaloReco" )
# # without merging:
# connectAlgorithmsIO( consumer=( efCaloReco, "RoIsContainer" ), producer=( l2CaloStep, "OutputHypoDecisions" ) )
# #efCaloReco.RoIsContainer = getOutput( l2CaloStep, "OutputDecisions" )
# #efCaloReco.RoIsContainer = getOutput( mergeAlgo, "MergedProxyContainer" )
# setOutput( efCaloReco, "OutputClusterContainer", "EFCaloFex" )
# setOutput( efCaloReco, "OutputProxyContainer", "EFCaloFexProxy" )
# efCaloReco.OutputLevel=DEBUG
# topSequence += efCaloReco


# efCaloHypo = TestHypoAlg( "EFCaloHypo" )
# efCaloHypo.OutputLevel=DEBUG
# connectAlgorithmsIO( consumer=( efCaloHypo, "InputProxyContainer" ), producer=( efCaloReco, "OutputProxyContainer" ) )
# #efCaloHypo.InputProxyContainer = getOutput( efCaloReco,"OutputProxyContainer" )
# setOutput( efCaloHypo, "OutputDecisions",efCaloHypo.name(  )+"Decisions" )


# from ViewAlgs.ViewAlgsConf import DumpDecisions
# efDumper = DumpDecisions( "EFCaloRoIsDumper" )
# efDumper.OutputLevel=VERBOSE
# connectAlgorithmsIO( consumer=( efDumper, "OutputDecisions" ), producer=( efCaloHypo, "OutputDecisions" ) )
# topSequence += efDumper





# # here we can have fake tracking
# # we ought to import merge RoIs alg which is fed by 
# # MuonRoIs produced by the muonFakeRoIs 
# # CaloRoIs produced by upstream alg l2CaloHypo ( reduced set for a change )
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
# mergeAlgo = TestMergeAlg( "MergeAllRoIs" )
# connectAlgorithmsIO( consumer=( mergeAlgo, "InpuCollections" ), producer=( l2CaloStep, "OutputHypoContainer" ), propset=False )
# connectAlgorithmsIO( consumer=( mergeAlgo, "InpuCollections" ), producer=( l2MuonStep, "OutputHypoContainer" ), propset=False )
# setOutput( mergeAlgo, "MergedProxyContainer", "MergedRoIs" )
# mergeAlgo.OutputLevel=DEBUG
# topSequence += mergeAlgo


# # then splitting again....
# # retrieve Calo RoIs



# # menu Algo for calo-tracking match
# #caloTrackingStep = genDecisionAlg( "MenuCaloTrackingStep" )#, [CaloMatchingHypo], [l2CaloStep] ) 
# connectHypoToMenu( consumer=menuStep3, producer=( CaloMatchingHypo, "OutputDecisions" ) )
# #caloTrackingStep.OutputLevel=VERBOSE
# #topSequence += caloTrackingStep


# CaloTrackingDumper = DumpDecisions( "CaloTrackingDumper" )
# CaloTrackingDumper.OutputLevel=VERBOSE
# connectAlgorithmsIO( consumer=( CaloTrackingDumper, "OutputDecisions" ), producer=( CaloMatchingHypo, "OutputDecisions" ) )
# #CaloTrackingDumper.OutputDecisions=getOutput( CaloMatchingHypo, "OutputDecisions" )
# topSequence += CaloTrackingDumper






##--------------------------
# Make the chains
##--------------------------

chains = ChainsList(  )
chains + Chain( "HLT_e3", "L1_EM3", 20.99, {"menuStep0": "EM3 x 1", 
                                            "menuStep1": "L2_3GeVCaloEtCut x 1", 
                                            "menuStep2": "EF_3GeVCaloEtCut x 1"} ) 

chains + Chain( "HLT_2e3", "L1_2EM3", 1, {"menuStep0": "EM3 x 2", 
                                          "menuStep1": "L2_3GeVCaloEtCut x 2",
                                          "menuStep2": "EF_3GeVCaloEtCut x 2"} )

chains + Chain( "HLT_e3e7", "L1_2EM3", 1, {"menuStep0": "EM3 x 1, EM7 x 1", 
                                           "menuStep1": "L2_3GeVCaloEtCut x 1, L2_7GeVCaloEtCut x 1",
                                           "menuStep2": "EF_3GeVCaloEtCut x 1, EF_7GeVCaloEtCut x 1"} )

chains + Chain( "HLT_e7", "L1_EM7", 1, {"menuStep0": "EM7 x 1", 
                                        "menuStep1": "L2_7GeVCaloEtCut x 1",
                                        "menuStep2": "EF_7GeVCaloEtCut x 1"} )

chains + Chain( "HLT_e10", "L1_EM10", 1, {"menuStep0": "EM10 x 1", 
                                          "menuStep1": "L2_10GeVCaloEtCut x 1",
                                          "menuStep2": "EF_10GeVCaloEtCut x 1"} )

chains + Chain( "HLT_mu6", "L1_MU6", 1, {"menuStep0": "MU6 x 1", 
                                         "menuStep1": "L2_6GeVMuonCut x 1"} )

chains + Chain( "HLT_mu6e3", "L1_EM3MU6", 1, {"menuStep0": "EM3 x 1", 
                                              "menuStep1": "MU6 x 1",
                                              "menuStep2": "L2_3GeVCaloEtCut x 1, L2_6GeVMuonCut x 1"}  )

chains + Chain( "HLT_e3merged", "L1_EM3", 20.99, {"menuStep0": "EM3 x 1", 
                                                  "menuStep1": "L2_3GeVCaloEtCut x 1", 
                                                  "menuStep2": "EF_3GeVCaloEtCut x 1"} ) 

chains + Chain( "HLT_e7merged", "L1_EM7", 1, {"menuStep0": "EM7 x 1", 
                                              "menuStep1": "L2_7GeVCaloEtCut x 1",
                                              "menuStep2": "EF_7GeVCaloEtCut x 1"} )

chains + Chain( "HLT_e7mu6_merged", "L1_EM7,L1_MU6", 1, {"menuStep0": "EM7 x 1, MU6 x 1", 
                                                         "menuStep1": "L2_7GeVCaloEtCut x 1",
                                                         "menuStep2": "L2_6GeVMuonCut x 1",
                                                         "menuStep3": "EF_7GeVCaloEtCut x 1"} )
 

# configureMenu( chains, topSequence )

# this is not yet working allright
#from ViewAlgsTest.Menu import menu_ingredients
#configureFromXML(  "HLTTestMenu.xml", menu_ingredients, topSequence )


#--------------------------------------------------------------
# Set output level threshold ( 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = DEBUG
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=True
#print svcMgr
MessageSvc.debugLimit=1000000
theApp.EvtMax = 4
theApp.SkipEvents=3

print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################

graph_generator.saveGraph( 'menuRun dependencies graph', 'graph.dot' )
