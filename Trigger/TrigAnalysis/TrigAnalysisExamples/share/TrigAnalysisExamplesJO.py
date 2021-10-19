
#
# Properties that should be modified by the user:
#
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if not "InputFiles" in dir():
    #InputFiles = ["/afs/cern.ch/user/r/rwhite/workspace/public/tutorial/mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.merge.AOD.e3601_s2876_r7917_r7676/AOD.08275487._000742.pool.root.1"]
    InputFiles = ["/afs/cern.ch/user/r/rwhite/workspace/public/tutorial/data16_13TeV.00307195.physics_Main.merge.DAOD_EGZ.f731_m1616_f731_m1662._0001.1"]
if not "OutputFile" in dir():
    OutputFile = "TDTExample.root"
if not "athenaCommonFlags.EvtMax" in dir():
    athenaCommonFlags.EvtMax = 100

#if not already done, you should determine if you are running on MC or Data:
#from PyUtils import AthFile
#af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0]) 
#isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']
isMC = True
#must set the globalflags appropriately so that the correct conditions database is queried when you extract the bunch train
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'geant4' if isMC else 'data' 
globalflags.DatabaseInstance = 'CONDBR2' 

# Set up the needed RecEx flags:
athenaCommonFlags.FilesInput.set_Value_and_Lock( InputFiles )

# Set up what the RecExCommon job should and shouldn't do:
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = [ "everything" ]
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteESD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doESD.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doDPD.set_Value_and_Lock( False )
rec.doHist.set_Value_and_Lock( False )
rec.doPerfMon.set_Value_and_Lock( False )
rec.doForwardDet.set_Value_and_Lock( False )

# Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

# Correctly configure the trigger via TriggerJobOpts
# The following will configure the TDT
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
TriggerConfigGetter()

#########################################################################
#                                                                       #
#                      Now set up the example job                       #
#                                                                       #
#########################################################################
# Define sets of triggers to use

electronHLTList=["HLT_e26_lhtight_nod0_ivarloose","HLT_e26_lhtight_nod0_iloose","HLT_e17_lhloose_nod0"] 
tauHLTList=[]
muonHLTList=[]
jetHLTList=["HLT_j300","HLT_3j20"]
combinedList=[]
mergedList=electronHLTList + tauHLTList + muonHLTList + jetHLTList

# Add the additional tools required for running the example algorithms

# Helper tool, provides some simple template methods to dump Navigation of chains
ToolSvc += CfgMgr.Trig__TriggerAnalysisHelper("TriggerAnalysisHelper")

# Matching Tool
ToolSvc += CfgMgr.Trig__MatchingTool("MatchingTool",OutputLevel=DEBUG)

# Emulators
#
# Egamma emulator -- disabled for now
# from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationToolFactory
# egEmulator = TrigEgammaEmulationToolFactory(elHLTList, name = "TrigEgammaEmulationTool",  OutputLevel = INFO)

#
# Trigger Analysis Examples
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import Trig__TDTExample
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TagAndProbeExAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import JetMetExAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigEmulatorExAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigExpressStreamAlg
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TriggerAnalysisTutorial
from TrigAnalysisExamples.TrigAnalysisExamplesConf import TrigMatchExAlg

tdtexample = Trig__TDTExample( "TDTExample", TriggerList=["HLT_e26_lhtight_nod0_ivarloose","HLT_e26_lhtight_nod0_iloose","HLT_e17_lhloose_nod0"] )
match = TrigMatchExAlg()
tp_electron=TagAndProbeExAlg( "TagAndProbeExAlg", Flavor="Electron",
        HLTTriggerList=electronHLTList,
        L1TriggerList=["L1_EM15VH","L1_EM22VHI"])
jetmet = JetMetExAlg("JetMetExAlg", HLTTriggerList=['HLT_j300'])
emulator = TrigEmulatorExAlg(L1TriggerList=["L1_TAU12", "L1_TAU12IM", "L1_TAU60"], HLTTriggerList=[])
express=TrigExpressStreamAlg()
tat = TriggerAnalysisTutorial()

# Histogram routing
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += ["Trigger DATAFILE='TriggerAnalysisTutorial.root' TYP='ROOT' OPT='RECREATE'"]
ServiceMgr.THistSvc.OutputLevel = ERROR

# Add the examples to the top algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += tdtexample
topSequence += match
topSequence += tp_electron
topSequence += jetmet
topSequence += emulator
topSequence += express
#topSequence += tat

