# Job options for standalone and Tier0 running of AnalysisTools 
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# 
# Tool and algorithm configuration done using egamma Factories
# Default configurations found in TrigEgammaAnalysisToolsConfig
#
# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/work/j/jolopezl/datasets/valid1.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3099_s2578_r6220_tid05203475_00'" 
#                 -c "NOV=50" test_ZeeElectronLowPtSupportingTrigAnalysis.py
# where NOV is the number of events to run

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AppMgr import ToolSvc
import os

doZ=True
doFakes=True

# Define triggers for tagging 
triggerTags     = ['HLT_e24_lhtight_nod0_ivarloose',
                   'HLT_e26_lhtight_nod0_ivarloose',
                   'HLT_e28_lhtight_nod0_ivarloose']

triggerList = [
         # standard monitoring chains
         'HLT_e26_lhtight_nod0_ivarloose',
         'HLT_e28_lhtight_nod0_ivarloose',
         'HLT_e60_lhmedium_nod0_L1EM24VHI',
         # no ringer chains for pos-commissioning...
         'HLT_e26_lhtight_nod0_noringer_ivarloose_L1EM22VHI',
         'HLT_e28_lhtight_nod0_noringer_ivarloose', # get real TDT
         'HLT_e28_lhtight_nod0_noringer_ivarloose_L1EM24VHI',
         'HLT_e60_lhmedium_nod0_noringer_L1EM24VHI',
      ]



#print 'Set some variables for job'
dirtouse = str()
finallist=[]

if 'FILE' in dir() :
     finallist.append(FILE)

if 'DIR' in dir() :
     dirtouse=DIR      
     print 'DIR = ',dirtouse
     while( dirtouse.endswith('/') ) :
          dirtouse= dirtouse.rstrip('/')
     listfiles=os.listdir(dirtouse)
     for ll in listfiles:
          finallist.append(dirtouse+'/'+ll)

if 'NOV' in dir():
    nov=NOV
else :
    nov=-1


print  finallist 
athenaCommonFlags.FilesInput=finallist
athenaCommonFlags.EvtMax=nov
rec.readAOD=True
# switch off detectors
rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False
rec.doEgamma=False
rec.doTrigger = True; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched off)
rec.doMuon=False
rec.doAOD=False
rec.doMuonCombined=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doDPD=False
rec.doTruth=False

# autoconfiguration might trigger undesired feature
rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(True) # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
MessageSvc.debugLimit = 20000000
MessageSvc.infoLimit  = 20000000
# TDT
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool( "TrigDecisionTool" )
tdt.TrigDecisionKey='xTrigDecision'
tdt.Navigation.ReadonlyHolders=True
ToolSvc+=tdt

try:
  include ("AthenaMonitoring/DataQualityInit_jobOptions.py")
except Exception:
  treatException("Could not load AthenaMonitoring/DataQualityInit_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

################ Mon Tools #################
#Global HLTMonTool

from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
HLTMon = HLTMonTool(name  = 'HLTMon', histoPathBase = "HLT");

ToolSvc += HLTMon;
HLTMonManager.AthenaMonTools += [ "HLTMonTool/HLTMon" ];
    
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaPlotTool, getEventSelectionTool
#Define the base path for all histograms
basePath = '/HLT/Physval'

#Configure the TrigEgammaPlotTool
PlotTool = TrigEgammaPlotTool.copy( name="TrigEgammaPlotTool",
                                    DirectoryPath=basePath,
                                    #MaM=monitoring_mam,
                                    Efficiency=[],
                                    Distribution=[],
                                    Resolution=[])

TrigEgammaEventSelection = getEventSelectionTool()

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


Tool = TrigEgammaEventSelection( name='EventSelection',
                              Analysis='Probes',
                              PlotTool=PlotTool,
                              EmulationTool=EmulationElectronTool,
                              File="",
                              OutputLevel=0,
                              DetailedHistograms=False,
                              isEMResultNames=["Tight","Medium","Loose"],
                              LHResultNames=["LHTight","LHMedium","LHLoose"],
                              ZeeLowerMass=80,
                              ZeeUpperMass=100,
                              OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose
                              OfflineProbeSelector='Loose', 
                              ForceProbePid=False, 
                              OppositeCharge=True,
                              OfflineTagMinEt=25,
                              TagTriggerList=triggerTags,
                              SelectionZ=doZ,
                              SelectionFakes=doFakes,
                              ApplyMinimalTrigger=False,
                              DetailedDataLevel=2,
                              DumpTags=False,
                              TriggerList=triggerList)

Tools=['TrigEgammaEventSelection/EventSelection']

from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
TrigEgammaMonTool = TrigEgammaMonTool( name = "HLTEgammaMon", 
                                       histoPathBase=basePath,
                                       Tools=Tools)

ToolSvc += TrigEgammaMonTool

#TrigEgammaMonToolConfig.TrigEgammaMonTool()
HLTMonManager.AthenaMonTools += [ "TrigEgammaMonTool/HLTEgammaMon" ]
HLTMonManager.FileKey = "GLOBAL"


    
