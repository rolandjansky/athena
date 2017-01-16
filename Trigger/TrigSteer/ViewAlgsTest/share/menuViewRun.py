#
# get_files -xmls LVL1config_Physics_pp_v5.xml
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
useView=True
#useView=False
#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------
## get a handle on the ServiceManager

from AthenaCommon.AppMgr import ServiceMgr as svcMgr


#
from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.CheckDependencies = True

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob
#


## basic job configuration
#import AthenaCommon.AtlasUnixStandardJob



## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()




from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = "LVL1config_Physics_pp_v5.xml"
svcMgr += l1svc


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them

from ViewAlgsTest.Menu import *
from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO, graph_generator


views =["CaloView", "MuonView"]
CALO=0
MUON=1

#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI, AthViews__FakeRoIView

caloFakeRoI = AthViews__FakeRoIView("caloFakeRoI")
if useView: caloFakeRoI.ViewName=views[CALO]
#caloFakeRoI.ExtraOutputs =  [( views[CALO] + '_' +  "CaloRoIs" )]
#[ ( 'int', manualViewName1 + '_view_start' ), ( 'int', manualViewName2 + '_view_start' ) ]
caloFakeRoI.InputFilename="caloRoIData.dat"
setOutput(caloFakeRoI, "OutputDecisions", "CaloRoIs")
setOutput(caloFakeRoI, "OutputRoIs", "CaloTrigRoIs")
setOutput(caloFakeRoI, "OutputRecEMTauRoIs", "CaloRecRoIs")
caloFakeRoI.OutputLevel=DEBUG
topSequence += caloFakeRoI

## muonFakeRoI = AthViews__FakeRoIView("muonFakeRoI")
## if useView: muonFakeRoI.ViewName=views[MUON]
muonFakeRoI = FakeRoI("muonFakeRoI")
muonFakeRoI.InputFilename="muonRoIData.dat"
setOutput(muonFakeRoI, "OutputDecisions", "MuonRoIs")
setOutput(muonFakeRoI, "OutputRoIs", "MuonTrigRoIs")
setOutput(muonFakeRoI, "OutputRecEMTauRoIs", "MuonRecRoIs")
muonFakeRoI.OutputLevel=DEBUG
topSequence += muonFakeRoI


from ViewAlgsTest.ViewAlgsTestConf import TestFEXAlg, AthViews__TestFEXAlgView
l2CaloReco = AthViews__TestFEXAlgView("L2CaloReco")
if useView: l2CaloReco.ViewName=views[CALO]
# exclusive running on the views
#l2CaloReco.RequireView = True
#l2CaloReco = TestFEXAlg("L2CaloReco")
connectAlgorithmsIO(consumer=(l2CaloReco, "RoIsContainer"), producer=(caloFakeRoI, "OutputDecisions"))
#connectAlgorithmsIO(consumer=(l2CaloReco, "RoIsContainer"), producer=(caloRoIsAfterPrescaling, "OutputDecisions"))
setOutput(l2CaloReco, "OutputClusterContainer", "L2CaloFex")
setOutput(l2CaloReco, "OutputProxyContainer", "L2CaloFexProxy")
l2CaloReco.OutputLevel=DEBUG
topSequence += l2CaloReco

from ViewAlgsTest.ViewAlgsTestConf import TestHypoAlg, AthViews__TestHypoAlgView, TestHypoTool
l2CaloHypo = AthViews__TestHypoAlgView("L2CaloHypo")
if useView: l2CaloHypo.ViewName=views[CALO]
#l2CaloHypo.RequireView = True
#l2CaloHypo = TestHypoAlg("L2CaloHypo")
l2CaloHypo.OutputLevel=DEBUG
connectAlgorithmsIO(consumer=(l2CaloHypo, "InputProxyContainer"), producer=(l2CaloReco, "OutputProxyContainer"))
#l2CaloHypo.InputProxyContainer = getOutput(l2CaloReco,"OutputProxyContainer")
setOutput(l2CaloHypo, "OutputDecisions",l2CaloHypo.name()+"Decisions")

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
topSequence += l2CaloHypo


#from ViewAlgs.ViewAlgsConf import MenuAlg, AthViews__MenuAlgView
#l2CaloStep = genMenuAlgView("MenuL2CaloStep", l2CaloHypo, caloRoIsAfterPrescaling)
#l2CaloStep.OutputLevel=VERBOSE
#topSequence += l2CaloStep

#Dumper
from ViewAlgs.ViewAlgsConf import DumpDecisions
l2Dumper = DumpDecisions("L2CaloRoIsDumper")
l2Dumper.OutputLevel=VERBOSE
connectAlgorithmsIO(consumer=(l2Dumper, "OutputDecisions"), producer=(l2CaloHypo, "OutputDecisions"))
l2Dumper.OutputDecisions=getOutput(l2CaloHypo, "OutputDecisions")
#topSequence += l2Dumper


# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=True
print svcMgr
MessageSvc.debugLimit=1000000
theApp.EvtMax = 4
theApp.SkipEvents=3

print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################

