
####################################################################################################
# Job options for standalone and Tier0 running of AnalysisTools 
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# Joao Victor Pinto    <jodafons@cern.ch>
# 
# Tool and algorithm configuration done using egamma Factories
#
# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/work/j/jolopezl/datasets/\
#                     valid1.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3099_s2578_r6220_tid05203475_00'" 
#                 -c "NOV=50" test_ZeeElectronLowPtSupportingTrigAnalysis.py
# where NOV is the number of events to run
####################################################################################################


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AppMgr import ToolSvc
import os

# Define triggers for tagging 
tagList = ['HLT_e24_lhtight_nod0_ivarloose',
           'HLT_e26_lhtight_nod0_ivarloose',
           'HLT_e28_lhtight_nod0_ivarloose']

triggerList_commissioning_v1 = [
          
         # standard monitoring chains
         #'HLT_e17_lhvloose_nod0_L1EM15VHI',
         'HLT_e17_lhvloose_nod0',
         'HLT_e26_lhloose_nod0',
         'HLT_e26_lhmedium_nod0',
         #'HLT_e26_lhtight_nod0',
         #'HLT_e26_lhtight_nod0_ivarloose',
         'HLT_e28_lhtight_nod0_ivarloose',
         'HLT_e60_lhmedium_nod0_L1EM24VHI',
         'HLT_e140_lhloose_nod0_L1EM24VHI',
         # ringer chains for the commissioning...
         #'HLT_e17_lhvloose_nod0_ringer_L1EM15VHI',
         'HLT_e17_lhvloose_nod0_ringer',
         'HLT_e26_lhloose_nod0_ringer',
         'HLT_e26_lhmedium_nod0_ringer',
         #'HLT_e26_lhtight_nod0_ringer',
         #'HLT_e26_lhtight_nod0_ringer_ivarloose',
         'HLT_e28_lhtight_nod0_ringer_ivarloose',
         'HLT_e60_lhmedium_nod0_ringer_L1EM24VHI',
         'HLT_e140_lhloose_nod0_ringer_L1EM24VHI',
    ]


triggerList_monitoring_v1 = [
         # standard monitoring chains
         'HLT_e17_lhvloose_nod0_L1EM15VHI',
         'HLT_e26_lhtight_nod0_ivarloose',
         'HLT_e28_lhtight_nod0_icaloloose',
         'HLT_e60_lhmedium_nod0_L1EM24VHI',
         'HLT_e140_lhloose_nod0_L1EM24VHI',
         # no ringer chains for pos-commissioning...
         'HLT_e17_lhvloose_nod0_noringer_L1EM15VHI',
         'HLT_e26_lhtight_nod0_noringer_ivarloose',
         'HLT_e28_lhtight_nod0_noringer_ivarloose',
         'HLT_e60_lhmedium_nod0_noringer_L1EM24VHI',
         'HLT_e140_lhloose_nod0_noringer_L1EM24VHI',

      ]


triggerList = triggerList_monitoring_v1

doGRL=False
doEmulation=True
DetailedHistograms=False
####################################################################################################
dirtouse = str()
finallist=[]
grllist=[]

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from glob import glob

if ('FILE' in dir()):
    acf.FilesInput=[FILE]
elif ('DIR' in dir()):
    inputFiles = glob(DIR+'*')
    acf.FilesInput=inputFiles
if 'NOV' in dir():
    nov=NOV
else :
    nov=500
if 'GRL' in dir():
  grllist.append(GRL)
else:
  #grllist.append('/afs/cern.ch/work/j/jodafons/public/data/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')
  #grllist.append('data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')
  #grllist.append('/afs/cern.ch/work/j/jodafons/public/data/data16_13TeV.periodAllYear_DetStatus-v81-pro20-10_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')
  #grllist.append('data16_13TeV.periodAllYear_DetStatus-v81-pro20-10_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')
  grllist.append('data17_13TeV.periodAllYear_DetStatus-v92-pro21-05_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml')

####################################################################################################

acf.EvtMax=nov
rec.readAOD=True
# switch off detectors
rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False
rec.doEgamma=False
rec.doTrigger=True
recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched off)
rec.doMuon=False
rec.doMuonCombined=False
rec.doWriteAOD=True
rec.doWriteESD=False
rec.doDPD=False
rec.doTruth=False
# autoconfiguration might trigger undesired feature
rec.doESD.set_Value_and_Lock(False)      # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(True)       # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
MessageSvc.debugLimit = 20000000
MessageSvc.infoLimit  = 20000000

# Add Trigger Decision Tool 
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool( "TrigDecisionTool" )
tdt.TrigDecisionKey='xTrigDecision'
tdt.Navigation.ReadonlyHolders=True
ToolSvc+=tdt

try:
  include ("AthenaMonitoring/DataQualityInit_jobOptions.py")
except Exception:
  treatException("Could not load AthenaMonitoring/DataQualityInit_jobOptions.py")




# Call top sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()


##################################### GRL Tools ##########################################
# Good Run List (GRL)
if doGRL:
  from RecExConfig.InputFilePeeker import inputFileSummary
  try:
    if inputFileSummary['evt_type'][0] == "IS_DATA":
      from GoodRunsLists.GoodRunsListsConf import *
      ToolSvc += GoodRunsListSelectorTool()
      GoodRunsListSelectorTool.GoodRunsListVec = grllist 
      from GoodRunsListsUser.GoodRunsListsUserConf import *
      seq = AthSequencer("AthFilterSeq")
      seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
      #seq.GRLTriggerAlg1.GoodRunsListArray = ['PHYS_StandardGRL_All_Good_25ns']  
      seq.GRLTriggerAlg1.GoodRunsListArray = [grllist[0].replace('.xml','')]  
  except:
    print "GRL not available!"
  
##################################### Mon Tools ##########################################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager
################ Mon Tools #################
#Global HLTMonTool
from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
HLTMon = HLTMonTool(name  = 'HLTMon', histoPathBase = "HLT");
ToolSvc += HLTMon;
HLTMonManager.AthenaMonTools += [ "HLTMonTool/HLTMon" ];

####################################################################################################
#Define the base path for all histograms



basePath = '/HLT/Egamma'
#Configure the TrigEgammaPlotTool
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavTPAnalysisTool, TrigEgammaPlotTool, EfficiencyTool, DistTool, ResolutionTool
from TrigEgammaAnalysisTools.TrigEgammaProbelist           import monitoring_mam, monitoring_electron, monitoring_photon 
from TrigEgammaAnalysisTools.TrigEgammaProbelist           import probeListLowMidPtSupportingTriggers, probeListHighPtSupportingTriggers

PlotTool = TrigEgammaPlotTool.copy( name="TrigEgammaPlotTool",
                                    DirectoryPath=basePath,
                                    MaM=monitoring_mam,
                                    Efficiency=[],
                                    Distribution=[],
                                    Resolution=[])

EffTool = EfficiencyTool.copy(name="EfficiencyTool",
                              PlotTool=PlotTool,
                              isEMResultNames=["Tight","Medium","Loose"],
                              LHResultNames=["LHTight","LHMedium","LHLoose"],
                              OutputLevel=0)



####################################################################################################
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationTool
from TrigEgammaEmulationTool.TrigEgammaEmulationEFConfig   import EgammaEFElectronDefaultEmulator,EgammaEFElectronNoD0Emulator
from TriggerJobOpts.TriggerFlags import TriggerFlags

# Force this emulation selector path
TriggerFlags.EgammaSlice.pidVersion.set_Value_and_Lock("ElectronPhotonSelectorTools/trigger/rel21_20170217/")
TriggerFlags.EgammaSlice.ringerVersion.set_Value_and_Lock("RingerSelectorTools/TrigL2_20170505_v6")

EmulationElectronTool = TrigEgammaEmulationTool.copy( name="TrigEgammaEmulationTool",
                                                      TriggerList = triggerList,
                                                      DoL2ElectronFex=True,
                                                      DoEFCaloPid=False,
                                                      DoRinger=True,
                                                      OutputLevel=0)

####################################################################################################

AnalysisTool = TrigEgammaNavTPAnalysisTool( name = "TrigEgammaNavTPAnalysisTool",
                                            Analysis='Zee',
                                            PlotTool=PlotTool,
                                            Tools=[EffTool],
                                            OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose
                                            OfflineProbeSelector='Loose', 
                                            DefaultProbePid='LHMedium',
                                            doEmulation=doEmulation,
                                            DetailedHistograms=DetailedHistograms,
                                            EmulationTool=EmulationElectronTool,
                                            TPTrigger=False,
                                            RemoveCrack=False,
                                            TagTriggerList=tagList,
                                            TriggerList=triggerList,
                                            OutputLevel=0)

####################################################################################################
Tools=['TrigEgammaNavTPAnalysisTool/TrigEgammaNavTPAnalysisTool']
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
TrigEgammaMonTool = TrigEgammaMonTool( name = "HLTEgammaMon", histoPathBase=basePath, Tools=Tools)
ToolSvc += TrigEgammaMonTool
HLTMonManager.AthenaMonTools += [ "TrigEgammaMonTool/HLTEgammaMon" ]
HLTMonManager.FileKey = "GLOBAL"
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = WARNING
####################################################################################################
