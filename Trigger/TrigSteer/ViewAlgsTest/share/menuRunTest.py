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


## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()


from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = "LVL1config_Physics_pp_v5.xml"
svcMgr += l1svc


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from ViewAlgsTest.Menu import *


from L1Decoder.L1DecoderConf import FakeCTP
fakeCTP = FakeCTP("FakeCTP")
fakeCTP.OutputLevel=VERBOSE
fakeCTP.InputFilename="menuData.dat"
fakeCTP.OutputLevel=VERBOSE


from ViewAlgs.ViewAlgsConf import PrescaleDecision
ps = PrescaleDecision("HLTChainsPrescaling")
setOutput(ps, "OutputChainDecisions", "ChainsPassingAfterPS")
ps.OutputLevel=DEBUG


#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
caloFakeRoI = FakeRoI("caloFakeRoI")
caloFakeRoI.InputFilename="caloRoIData.dat"
setOutput(caloFakeRoI, "OutputDecisions", "CaloRoIs")
setOutput(caloFakeRoI, "OutputRoIs", "CaloTrigRoIs")
setOutput(caloFakeRoI, "OutputRecEMTauRoIs", "CaloRecRoIs")
caloFakeRoI.OutputLevel=DEBUG



muonFakeRoI = FakeRoI("muonFakeRoI")
muonFakeRoI.InputFilename="muonRoIData.dat"
setOutput(muonFakeRoI, "OutputDecisions", "MuonRoIs")
setOutput(muonFakeRoI, "OutputRoIs", "MuonTrigRoIs")
setOutput(muonFakeRoI, "OutputRecEMTauRoIs", "MuonRecRoIs")
muonFakeRoI.OutputLevel=DEBUG


from ViewAlgs.ViewAlgsConf import CopyPassing

# Calo L1 RoIs, used by Fexes
caloRoIsAfterPrescaling = CopyPassing("copyCaloRoIsPassingPrecaling")
setOutput(caloRoIsAfterPrescaling, "Output", "CaloRoIsAfterPrescaling" )
caloRoIsAfterPrescaling.OutputLevel=DEBUG


# Muon L1 RoIs, used by Fexes
muonRoIsAfterPrescaling = CopyPassing("copyMuonRoIsPassingPrecaling")
setOutput(muonRoIsAfterPrescaling, "Output", "MuonRoIsAfterPrescaling" )
muonRoIsAfterPrescaling.OutputLevel=DEBUG


###########
# Start L2
###########

# calo L2 algorithm ------------
from ViewAlgsTest.ViewAlgsTestConf import TestFEXAlg
l2CaloReco = TestFEXAlg("L2CaloReco")
setOutput(l2CaloReco, "OutputClusterContainer", "L2CaloFex")
setOutput(l2CaloReco, "OutputProxyContainer", "L2CaloFexProxy")
l2CaloReco.OutputLevel=DEBUG


from ViewAlgsTest.ViewAlgsTestConf import TestHypoAlg, TestHypoTool
l2CaloHypo = TestHypoAlg("L2CaloHypo")
setOutput(l2CaloHypo, "OutputDecisions",l2CaloHypo.name()+"Decisions")
l2CaloHypo.OutputLevel=DEBUG


l2cut3 = TestHypoTool("L2_3GeVCaloEtCut", EtThreshold=3.3)
l2cut7 = TestHypoTool("L2_7GeVCaloEtCut", EtThreshold=7.2)
l2cut10 = TestHypoTool("L2_10GeVCaloEtCut", EtThreshold=10.4)
l2cut12 = TestHypoTool("L2_12GeVCaloEtCut", EtThreshold=12.2)
# cuts 
l2CaloHypo += l2cut3
l2CaloHypo += l2cut7
l2CaloHypo += l2cut10
l2CaloHypo += l2cut12
l2CaloHypo.Hypotheses = [l2cut3, l2cut7, l2cut10, l2cut12]


#Dumper
from ViewAlgs.ViewAlgsConf import DumpDecisions
l2Dumper = DumpDecisions("L2CaloRoIsDumper")
l2Dumper.OutputLevel=VERBOSE


# muon L2 Algorithm -------------------------
l2MuonReco = TestFEXAlg("L2MuonReco")
setOutput(l2MuonReco, "OutputClusterContainer", "L2MuonFex")
setOutput(l2MuonReco, "OutputProxyContainer", "L2MuonFexProxy")
l2MuonReco.OutputLevel=DEBUG

l2MuonHypo = TestHypoAlg("L2MuonHypo")
l2MuonHypo.OutputLevel=DEBUG
setOutput(l2MuonHypo, "OutputDecisions",l2MuonHypo.name()+"Decisions")
l2cut6mu = TestHypoTool("L2_6GeVMuonCut", EtThreshold=6.0)

# cuts 
l2MuonHypo += l2cut6mu
l2MuonHypo.Hypotheses = [l2cut6mu]


# next step EF calo - better calo
efCaloReco = TestFEXAlg("EFCaloReco")
setOutput(efCaloReco, "OutputClusterContainer", "EFCaloFex")
setOutput(efCaloReco, "OutputProxyContainer", "EFCaloFexProxy")
efCaloReco.OutputLevel=DEBUG

efCaloHypo = TestHypoAlg("EFCaloHypo")
efCaloHypo.OutputLevel=DEBUG
setOutput(efCaloHypo, "OutputDecisions",efCaloHypo.name()+"Decisions")

# cuts 
efcut3 = TestHypoTool("EF_3GeVCaloEtCut", EtThreshold=3.3)
efcut7 = TestHypoTool("EF_7GeVCaloEtCut", EtThreshold=7.2)
efcut10 = TestHypoTool("EF_10GeVCaloEtCut", EtThreshold=10.4)
efcut12 = TestHypoTool("EF_12GeVCaloEtCut", EtThreshold=12.2)
efCaloHypo += efcut3
efCaloHypo += efcut7
efCaloHypo += efcut10
efCaloHypo += efcut12
efCaloHypo.Hypotheses = [efcut3, efcut7, efcut10, efcut12]


efDumper = DumpDecisions("EFCaloRoIsDumper")
efDumper.OutputLevel=VERBOSE

## # here we can have fake tracking
## # we ought to import merge RoIs alg which is fed by 
## # MuonRoIs produced by the muonFakeRoIs 
## # CaloRoIs produced by upstream alg l2CaloHypo (reduced set for a change)
## # result of it is merged/super RoIs set
## #
## # then we run tracking 
## #
## # and RoI splitter 

## ##--------------------------
## # test merging/splitting
## ##--------------------------
## #  merge of Calo RoIs

from ViewAlgsTest.ViewAlgsTestConf import TestMergeAlg
mergeAlgo = TestMergeAlg("MergeAllRoIs")
setOutput(mergeAlgo, "MergedProxyContainer", "MergedRoIs")
mergeAlgo.OutputLevel=DEBUG

# next step - add here tracking on the merged Container (one RoI)
# fake tracking now
MergedTrackReco = TestFEXAlg("MergedTracking")
setOutput(MergedTrackReco, "OutputClusterContainer", "MergedTrackCollection") 
setOutput(MergedTrackReco, "OutputProxyContainer", "MergedTrackFexProxy")
MergedTrackReco.OutputLevel=DEBUG

# then splitting again....
# retrieve Calo RoIs
from ViewAlgsTest.ViewAlgsTestConf import TestSplitAlg
splitCaloAlgo = TestSplitAlg("SplitCaloRoIs")
setOutput(splitCaloAlgo, "SplitProxyContainer", "SplitCaloRoIs")
splitCaloAlgo.OutputLevel=DEBUG

# retrieve Muon RoIs
splitMuonAlgo = TestSplitAlg("SplitMuonRoIs")
setOutput(splitMuonAlgo, "SplitProxyContainer", "SplitMuonRoIs")
splitMuonAlgo.OutputLevel=DEBUG


# do track-cluster match
CaloMatchingFex = TestFEXAlg("CaloMatchingFex")
setOutput(CaloMatchingFex, "OutputClusterContainer", "CaloMatchingCollection") 
setOutput(CaloMatchingFex, "OutputProxyContainer", "CaloMatchingFexProxy")
CaloMatchingFex.OutputLevel=DEBUG


# calo-mathcing hypo
CaloMatchingHypo = TestHypoAlg("CaloMatchingHypo")
CaloMatchingHypo.OutputLevel=DEBUG
setOutput(CaloMatchingHypo, "OutputDecisions",CaloMatchingHypo.name()+"Decisions")

# cuts 
CaloMatchingHypo += efcut3
CaloMatchingHypo += efcut7
CaloMatchingHypo += efcut10
CaloMatchingHypo += efcut12
CaloMatchingHypo.Hypotheses = [efcut3, efcut7, efcut10, efcut12]

CaloTrackingDumper = DumpDecisions("CaloTrackingDumper")
CaloTrackingDumper.OutputLevel=VERBOSE



# create sequence of algorithm from menu
from MenuMaker import MenuMaker
MenuMaker(job, VERBOSE)







#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=True
#print svcMgr
MessageSvc.debugLimit=1000000
theApp.EvtMax = 4
#theApp.SkipEvents=3

print "topSequence dump:", job
#
# End of job options file
#
###############################################################
from ViewAlgsTest.connectAlgorithmsIO import  graph_generator
graph_generator.saveGraph('menuRun dependencies graph', 'graph.dot')
