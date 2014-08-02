#import os
#mydir = "/scratch/dferreir/misal1_mc12.005802.JF17_pythia_jet_filter.digit.RDO.v12003105_tid004254/"
#PoolRDOInput = [(mydir+x) for x in os.listdir(mydir)]

WhichInput = "RDO"
RunOffline = False

DetDescrVersion = "ATLAS-CSC-01-02-00"
EvtMax = -1
OutputLevel = INFO

include("TrigEgammaValidation/generalJobOption.py")

theApp.serviceMgr().IOVDbSvc.GlobalTag="OFLCOND-CSC-00-01-00"

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigEgammaValidation.TrigEgammaValidationConf import JetValidation
myJetValidation = JetValidation()
topSequence += myJetValidation
#myJetValidation.OutputLevel = DEBUG

# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
tHistSvc = svcMgr.THistSvc
tHistSvc.Output += [ "Validation DATAFILE='Validation_j.root' OPT='RECREATE'" ]
theApp.HistogramPersistency = "ROOT"

# To Dump Truth:
#theApp.Dlls  += [ "TruthExamples" ]   
#theApp.TopAlg += ["DumpMC"]
#Algorithm("DumpMC").McEventKey = "TruthEvent"
#Algorithm("DumpMC").McEventOutputKey = "TruthEvent"

#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True

DetFlags.Print()

print theApp.TopAlg
print theApp.Dlls

import os;
f = file('file.txt', 'w');
f.write('Validation_j.root\n');
f.write('/afs/cern.ch/user/d/dferreir/public/references_new_dataset/TrigEgammaValidation_RTT_testOptions_j/reference.root\n');
f.close();

