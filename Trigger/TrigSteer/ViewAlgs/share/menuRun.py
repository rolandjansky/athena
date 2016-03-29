#
# get_files -xmls LVL1config_Physics_pp_v5.xml

# also needed are "data" files: menuData.dat and testData.dat located in L1Bdecoder/share
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

def setOutput(obj, prop, name):
    setattr(obj, prop, name)
    setattr(obj, prop+"Aux", name+"Aux.")

def getOutput(obj, prop):
    print "getOutput"
    try:
        print "getOutput getattr"
        return getattr(obj, prop)
    except:
        print "getOutput getDefaultProperty ", obj.getDefaultProperty(prop)
        return obj.getDefaultProperty(prop)
    print "Can not read pro"
    raise "Error in reading property " + prop + " from " + obj
 
#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
fakeRoI = FakeRoI("fakeRoI")
fakeRoI.InputFilename="testData.dat"
setOutput(fakeRoI, "OutputDecisions", "CaloRoIs")
fakeRoI.OutputLevel=DEBUG
topSequence += fakeRoI

from L1Decoder.L1DecoderConf import FakeCTP
fakeCTP = FakeCTP()
fakeCTP.OutputLevel=VERBOSE
fakeCTP.InputFilename="menuData.dat"
fakeCTP.OutputLevel=VERBOSE
topSequence += fakeCTP

from ViewAlgs.ViewAlgsConf import PrescaleDecision
ps  = PrescaleDecision("HLTChainsPrescaling")
ps.Prescales=["HLT_e3 20.99", "HLT_e7 2.5"]
ps.InputChainDecisions = getOutput(fakeCTP, "OutputDecisions")
setOutput(ps, "OutputChainDecisions", "ChainsPassingAfterPS")
ps.OutputLevel=DEBUG
topSequence += ps

from ViewAlgs.ViewAlgsConf import MenuAlg
dropPrescaled = MenuAlg("DropPrescaled")
dropPrescaled.HypoDecisions = getOutput(fakeRoI, "OutputDecisions")
dropPrescaled.InputChainDecisions = getOutput(ps,"OutputChainDecisions")
setOutput(dropPrescaled, "OutputDecisions", "RelevantEMRoIs")
setOutput(dropPrescaled, "OutputChainDecisions", "ChainsPassingAfterEM")
dropPrescaled.Required=["HLT_e7 = EM7 x 1", 
                        "HLT_2e7 = EM7 x 2", 
                        "HLT_e3 = EM3 x 1"]
topSequence += dropPrescaled

from ViewAlgs.ViewAlgsConf import TestFEXAlg
l2CaloReco = TestFEXAlg("L2CaloReco")
l2CaloReco.RoIsContainer = getOutput(dropPrescaled, "OutputDecisions")
setOutput(l2CaloReco, "Output", "L2CaloFex")
l2CaloReco.OutputLevel=DEBUG
topSequence += l2CaloReco

from ViewAlgs.ViewAlgsConf import TestHypoAlg, TestHypoTool
l2CaloHypo = TestHypoAlg("L1CaloHypo")
l2CaloHypo.OutputLevel=DEBUG
l2CaloHypo.ClusterContainer = getOutput(l2CaloReco,"Output")
setOutput(l2CaloHypo, "Output","L2CaloHypoDecisions")

# cuts 
cut3 = TestHypoTool("3GeVCaloEtCut", EtThreshold=0.3)
cut7 = TestHypoTool("7GeVCaloEtCut", EtThreshold=0.7)
cut10 = TestHypoTool("10GeVCaloEtCut", EtThreshold=0.10)
cut12 = TestHypoTool("12GeVCaloEtCut", EtThreshold=0.12)
l2CaloHypo += cut3
l2CaloHypo += cut7
l2CaloHypo += cut10
l2CaloHypo += cut12
l2CaloHypo.Hypotheses = [cut3, cut7, cut10, cut12]
topSequence += l2CaloHypo


from ViewAlgs.ViewAlgsConf import MenuAlg
caloStep = MenuAlg("CaloStep")
caloStep.HypoDecisions = l2CaloHypo.Output
caloStep.InputChainDecisions = getOutput(dropPrescaled, "OutputChainDecisions")
setOutput(caloStep, "OutputDecisions", "RelevantEMCaloRoIs")

setOutput(caloStep, "OutputChainDecisions", "ChainsPassingEMCaloStep")
caloStep.Required=["HLT_e7 = 7GeVCaloEtCut x 1", 
                   "HLT_2e7 = 7GeVCaloEtCut x 2", 
                   "HLT_e3 = 3GeVCaloEtCut x 1"]
caloStep.OutputLevel=VERBOSE
topSequence += caloStep

#Dumper
from ViewAlgs.ViewAlgsConf import DumpDecisions
dumper = DumpDecisions("CaloRoIsDumper")
dumper.OutputLevel=VERBOSE
dumper.OutputDecisions=getOutput(l2CaloHypo, "Output")
topSequence += dumper


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=True
print svcMgr
MessageSvc.debugLimit=1000000

theApp.EvtMax = 10


print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################
