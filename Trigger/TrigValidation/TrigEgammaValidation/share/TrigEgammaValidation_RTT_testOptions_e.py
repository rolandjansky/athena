#import os

#mydir = "/scratch/dferreir/mc08.005144.PythiaZee.digit.RDO.e323_s400_d99/"
#PoolRDOInput = [(mydir+x) for x in os.listdir(mydir)]

#PoolRDOInput  = []
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00001.pool.root.2"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00002.pool.root.6"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00003.pool.root.2"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00004.pool.root.2"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00005.pool.root.2"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00006.pool.root.2"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00007.pool.root.2"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00008.pool.root.2"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00009.pool.root.4"]
#PoolRDOInput += ["/rscratch/pcatr16/dferreir/valid1.005144.PythiaZee.digit.RDO.e322_s454/RDO.023689._00010.pool.root.2"]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

WhichInput = "RDO"
RunOffline = False

DetDescrVersion = "ATLAS-CSC-02-01-00"
EvtMax = -1
OutputLevel = INFO

include("TrigEgammaValidation/generalJobOption.py")

theApp.serviceMgr().IOVDbSvc.GlobalTag = "OFLCOND-CSC-00-01-00"

# Now load Validation algorithm:
from TrigEgammaValidation.TrigEgammaValidationConf import ZeeValidation
myZeeValidation = ZeeValidation()
topSequence += myZeeValidation
#myZeeValidation.OutputLevel = DEBUG

# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
tHistSvc = svcMgr.THistSvc
tHistSvc.Output += [ "Validation DATAFILE='Validation_e.root' OPT='RECREATE'" ]
theApp.HistogramPersistency = "ROOT"

# To Dump Truth:
#theApp.Dlls  += [ "TruthExamples" ]   
#theApp.TopAlg += ["DumpMC"]
#Algorithm("DumpMC").McEventKey = "TruthEvent"
#Algorithm("DumpMC").McEventOutputKey = "TruthEvent"

#svcMgr.StoreGateSvc.Dump = True

DetFlags.Print()

print theApp.TopAlg
print theApp.Dlls

import os;
f = file('file.txt', 'w');
f.write('Validation_e.root\n');
f.write('/afs/cern.ch/user/d/dferreir/public/references_new_dataset/TrigEgammaValidation_RTT_testOptions_e/reference.root\n');
f.close();

