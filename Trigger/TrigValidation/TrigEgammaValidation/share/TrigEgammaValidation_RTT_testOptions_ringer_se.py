WhichInput = "RDO"
RunOffline = False

DetDescrVersion = "ATLAS-CSC-05-00-00"
EvtMax = -1 
OutputLevel = INFO

doJetRec = False;
include("TrigEgammaValidation/generalJobOption.py")

theApp.serviceMgr().IOVDbSvc.GlobalTag="OFLCOND-CSC-01-00-00"

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMaker
tdm = TrigDecisionMaker()
topSequence += tdm

tdm.doL1 = True
tdm.doL2 = True
tdm.doEF = True

# Now load Validation algorithm:
from TrigDecision.TrigDecisionConf import TrigDec__TrigDecisionTool
tdt = TrigDec__TrigDecisionTool()
ToolSvc += tdt

from TrigEgammaValidation.TrigEgammaValidationConf import NavRingerValidation
nav = NavRingerValidation()
nav.Chains = ['e10Ringer']
nav.TrigDecisionTool = tdt
topSequence += nav

# setup TTree registration Service
# save ROOT histograms and Tuple
THistSvc = Service ( "THistSvc" )
THistSvc.Output += ["Validation DATAFILE='Validation_ringer_se.root' OPT='RECREATE'"]
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"

DetFlags.Print()

import os;
f = file('file.txt', 'w');
f.write('Validation_ringer_se.root\n');
f.write('/afs/cern.ch/user/d/dferreir/public/references_new_dataset/TrigEgammaValidation_RTT_testOptions_ringer_se/reference.root\n');
f.close();

