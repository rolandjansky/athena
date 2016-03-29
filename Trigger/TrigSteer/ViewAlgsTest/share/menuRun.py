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
    try:
        # print "getOutput getattr"
        return getattr(obj, prop)
    except:
        # print "getOutput getDefaultProperty ", obj.getDefaultProperty(prop)
        return obj.getDefaultProperty(prop)
    # print "Can not read pro"
    raise "Error in reading property " + prop + " from " + obj
 
def genMenuAlg(name, inputHypos, inputChains):
    assert inputHypos != None, 'Alg to take hypo putput from is missing'
    assert inputChains != None, 'Alg to take chain decisions from is missing'
    print inputChains, inputHypos
    from ViewAlgs.ViewAlgsConf import MenuAlg
    menuAlg = MenuAlg(name)
    menuAlg.HypoDecisions = getOutput(inputHypos, "OutputDecisions")
    menuAlg.InputChainDecisions = getOutput(inputChains,"OutputChainDecisions")
    setOutput(menuAlg, "OutputDecisions", name+"RoIs")
    setOutput(menuAlg, "OutputChainDecisions", name+"Chains")
    return menuAlg


#Run Fake RoI
from L1Decoder.L1DecoderConf import FakeRoI
fakeRoI = FakeRoI("fakeRoI")
fakeRoI.InputFilename="roiData.dat"
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
dropPrescaled = genMenuAlg("DropPrescaled", fakeRoI, ps)
dropPrescaled.Required=["HLT_2e3 = EM3 x 2", 
                        "HLT_e3 = EM3 x 1"
                        "HLT_e7 = EM7 x 1", 
                        "HLT_e10 = EM10 x 1"]
topSequence += dropPrescaled

from ViewAlgsTest.ViewAlgsTestConf import TestFEXAlg
l2CaloReco = TestFEXAlg("L2CaloReco")
l2CaloReco.RoIsContainer = getOutput(dropPrescaled, "OutputDecisions")
setOutput(l2CaloReco, "OutputClusterContainer", "L2CaloFex")
setOutput(l2CaloReco, "OutputProxyContainer", "L2CaloFexProxy")
l2CaloReco.OutputLevel=DEBUG
topSequence += l2CaloReco

from ViewAlgsTest.ViewAlgsTestConf import TestHypoAlg, TestHypoTool
l2CaloHypo = TestHypoAlg("L2CaloHypo")
l2CaloHypo.OutputLevel=DEBUG
l2CaloHypo.ClusterProxyContainer = getOutput(l2CaloReco,"OutputProxyContainer")
setOutput(l2CaloHypo, "OutputDecisions",l2CaloHypo.name()+"Decisions")

# cuts 
cut3 = TestHypoTool("L2_3GeVCaloEtCut", EtThreshold=3)
cut7 = TestHypoTool("L2_7GeVCaloEtCut", EtThreshold=7)
cut10 = TestHypoTool("L2_10GeVCaloEtCut", EtThreshold=10)
cut12 = TestHypoTool("L2_12GeVCaloEtCut", EtThreshold=12)
l2CaloHypo += cut3
l2CaloHypo += cut7
l2CaloHypo += cut10
l2CaloHypo += cut12
l2CaloHypo.Hypotheses = [cut3, cut7, cut10, cut12]
topSequence += l2CaloHypo


from ViewAlgs.ViewAlgsConf import MenuAlg
l2CaloStep = genMenuAlg("MenuL2CaloStep", l2CaloHypo, dropPrescaled)
l2CaloStep.Required=["HLT_2e3 = L2_3GeVCaloEtCut x 2", 
                     "HLT_e3 = L2_3GeVCaloEtCut x 1", 
                     "HLT_e7 = L2_7GeVCaloEtCut x 1",                     
                     "HLT_e10 = L2_10GeVCaloEtCut x 1"]
l2CaloStep.OutputLevel=VERBOSE
topSequence += l2CaloStep

#Dumper
from ViewAlgs.ViewAlgsConf import DumpDecisions
l2Dumper = DumpDecisions("L2CaloRoIsDumper")
l2Dumper.OutputLevel=VERBOSE
l2Dumper.OutputDecisions=getOutput(l2CaloHypo, "OutputDecisions")
topSequence += l2Dumper


# next step - better calo
efCaloReco = TestFEXAlg("EFCaloReco")
efCaloReco.RoIsContainer = getOutput(l2CaloStep, "OutputDecisions")
setOutput(efCaloReco, "OutputClusterContainer", "EFCaloFex")
setOutput(efCaloReco, "OutputProxyContainer", "EFCaloFexProxy")
efCaloReco.OutputLevel=DEBUG
topSequence += efCaloReco


efCaloHypo = TestHypoAlg("EFCaloHypo")
efCaloHypo.OutputLevel=DEBUG
efCaloHypo.ClusterProxyContainer = getOutput(efCaloReco,"OutputProxyContainer")
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
topSequence += efCaloHypo


efCaloStep = genMenuAlg("MenuEFCaloStep", efCaloHypo, l2CaloStep)
efCaloStep.Required=["HLT_2e3 = EF_3GeVCaloEtCut x 2", 
                   "HLT_e3 = EF_3GeVCaloEtCut x 1", 
                   "HLT_e7 = EF_7GeVCaloEtCut x 1",                     
                   "HLT_e10 = EF_10GeVCaloEtCut x 1"]
efCaloStep.OutputLevel=VERBOSE
topSequence += efCaloStep

from ViewAlgs.ViewAlgsConf import DumpDecisions
efDumper = DumpDecisions("EFCaloRoIsDumper")
efDumper.OutputLevel=VERBOSE
efDumper.OutputDecisions=getOutput(efCaloHypo, "OutputDecisions")
topSequence += efDumper


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

theApp.EvtMax = 3


print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################
