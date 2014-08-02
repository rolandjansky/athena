WhichInput = "RDO"

#import os
#directory = "/rscratch/pcatr17/dferreir/mc08.107020.singlepart_e_Et7-80.digit.RDO.e342_s439/"
#PoolRDOInput = [ ( directory + x )  for x in os.listdir(directory) ]

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

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel=WARNING
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = WARNING
tdt.PublicChainGroups = {"EFEgamma": "EF_e.*",
                         "L2Egamma": "L2_e.*"}

from TrigEgammaValidation.TrigEgammaValidationConf import NavOfflineValidation, NavRateValidation, NavRateExpert
nav = NavOfflineValidation()
nav.OutputLevel = DEBUG
nav.Chains = ['eNoCut', 
 'e10Ringer',
 'e5_medium',
 'e5_medium1',
 'e6_medium',
 'e6_medium1',
 'e7_medium',
 'e10_loose',
 'e10_medium',
 'e12_medium',
 'e15_medium',
 'e15_loose',
 'e15i_medium',
 'e15i_loose',
 'e17i_medium',
 'e20_loose',
 'e20_medium1',
 'e20i_loose',
 'e20i_medium1',
 'e25_loose',
 'e25_medium1',
 'e25i_loose',
 'e25i_medium1',
 'e22i_tight',
 'e55_loose1' ]
nav.TrigDecisionTool = tdt
rate = NavRateValidation()
rate.OutputLevel = DEBUG
rate.Chains = ['eNoCut', 
 'e10Ringer',
 'e5_medium',
 'e5_medium1',
 'e6_medium',
 'e6_medium1',
 'e7_medium',
 'e10_loose',
 'e10_medium',
 'e12_medium',
 'e15_medium',
 'e15_loose',
 'e15i_medium',
 'e15i_loose',
 'e17i_medium',
 'e20_loose',
 'e20_medium1',
 'e20i_loose',
 'e20i_medium1',
 'e25_loose',
 'e25_medium1',
 'e25i_loose',
 'e25i_medium1',
 'e22i_tight',
 'e55_loose1' ]
rate.TrigDecisionTool = tdt
expert = NavRateExpert()
expert.OutputLevel = DEBUG
expert.Chains = ['eNoCut', 
 'e10Ringer',
 'e5_medium',
 'e5_medium1',
 'e6_medium',
 'e6_medium1',
 'e7_medium',
 'e10_loose',
 'e10_medium',
 'e12_medium',
 'e15_medium',
 'e15_loose',
 'e15i_medium',
 'e15i_loose',
 'e17i_medium',
 'e20_loose',
 'e20_medium1',
 'e20i_loose',
 'e20i_medium1',
 'e25_loose',
 'e25_medium1',
 'e25i_loose',
 'e25i_medium1',
 'e22i_tight',
 'e55_loose1' ]
expert.TrigDecisionTool = tdt
topSequence += nav
topSequence += rate
topSequence += expert

# setup TTree registration Service
# save ROOT histograms and Tuple
THistSvc = Service ( "THistSvc" )
THistSvc.Output += ["Validation DATAFILE='Validation_off_nav.root' OPT='RECREATE'"]
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"

DetFlags.Print()
