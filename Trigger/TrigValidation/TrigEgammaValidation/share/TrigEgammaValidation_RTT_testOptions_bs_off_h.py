WhichInput = "BS"
RunOffline = True

DetDescrVersion = "ATLAS-GEO-02-01-00"
ConditionsTag = "OFLCOND-SIM-00-00-00"
EvtMax = -1
OutputLevel = INFO

include("TrigEgammaValidation/generalJobOption.py")

# Now load Validation algorithm:
from TrigEgammaValidation.TrigEgammaValidationConf import SingleElectronOfflineValidation

myHggOfflineValidation = SingleElectronOfflineValidation("HggOfflineValidation")
topSequence += myHggOfflineValidation
myHggOfflineValidation.OutputLevel = DEBUG

# setup TTree registration Service
# save ROOT histograms and Tuple
THistSvc = Service ( "THistSvc" )
THistSvc.Output = ["Validation DATAFILE='Validation_off_h.root' OPT='RECREATE'"]
theApp.Dlls += [ "RootHistCnv" ]
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
f = file('file.txt','w');
f.write('Validation_off_h.root\n');
f.write('/afs/cern.ch/user/d/dferreir/public/references_new_dataset/TrigEgammaValidation_RTT_testOptions_off_h/reference.root\n');
f.close();

