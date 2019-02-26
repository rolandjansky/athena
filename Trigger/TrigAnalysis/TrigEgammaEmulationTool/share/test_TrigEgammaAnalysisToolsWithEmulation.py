# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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



triggerList = [
               'HLT_e24_lhmedium_nod0_iloose',
               'HLT_e24_lhmedium_nod0_ivarloose',
               'HLT_e24_lhmedium_nod0_ringer_ivarloose',
               'HLT_e26_lhtight_nod0_ringer_ivarloose',
               'HLT_e60_lhmedium_nod0_ringer',
               'HLT_e140_lhloose_nod0_ringer',
               'HLT_e28_lhtight_nod0_ringer_ivarloose',
               'HLT_e60_lhmedium_nod0_ringer_L1EM24VHI',
               'HLT_e140_lhloose_nod0_ringer_L1EM24VHI',
               #'HLT_e17_lhloose_nod0_ringer_mu14',
               'HLT_e17_lhvloose_nod0_ringer',
               'HLT_e17_lhloose_nod0_ringer',
               
               'HLT_e24_lhmedium_nod0_ivarloose',
               'HLT_e26_lhtight_nod0_ivarloose',
               'HLT_e60_lhmedium_nod0',
               'HLT_e140_lhloose_nod0',
               'HLT_e28_lhtight_nod0_ivarloose',
               'HLT_e60_lhmedium_nod0_L1EM24VHI',
               'HLT_e140_lhloose_nod0_L1EM24VHI',
               #'HLT_e17_lhloose_nod0_mu14',
               'HLT_e17_lhvloose_nod0',
               'HLT_e17_lhloose_nod0'
              ]
doNewLH=True
####################################################################################################
dirtouse = str()
finallist=[]
grllist=[]
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

if 'GRL' in dir():
  grllist.append(GRL)
else:
  #grllist.append('/afs/cern.ch/work/j/jodafons/public/data/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')
  grllist.append('data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')
  #grllist.append('/afs/cern.ch/work/j/jodafons/public/data/data16_13TeV.periodAllYear_DetStatus-v81-pro20-10_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')
  grllist.append('data16_13TeV.periodAllYear_DetStatus-v81-pro20-10_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml')

####################################################################################################

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
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
# Call top sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()


##################################### GRL Tools ##########################################
# Good Run List (GRL)
from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary['evt_type'][0] == "IS_DATA":
  print 'IS_DATA! doGRL'
  from GoodRunsLists.GoodRunsListsConf import *
  ToolSvc += GoodRunsListSelectorTool()
  GoodRunsListSelectorTool.GoodRunsListVec = grllist 
  from GoodRunsListsUser.GoodRunsListsUserConf import *
  seq = AthSequencer("AthMasterSeq")
  seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
  seq.GRLTriggerAlg1.GoodRunsListArray = ['PHYS_StandardGRL_All_Good_25ns']  
##################################### Mon Tools ##########################################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager
################ Mon Tools #################
#Global HLTMonTool
from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTMonTool
HLTMon = HLTMonTool(name  = 'HLTMon', histoPathBase = "HLT");
ToolSvc += HLTMon;
HLTMonManager.AthenaMonTools += [ HLTMon ];

####################################################################################################
#Define the base path for all histograms



basePath = '/HLT/Egamma'
#Configure the TrigEgammaPlotTool
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavTPAnalysisTool, TrigEgammaPlotTool, EfficiencyTool
from TrigEgammaAnalysisTools.TrigEgammaProbelist           import monitoringTP_electron_extra,  triggerTags
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
# Overwrite all AsgSelectorTools
if doNewLH:
  from TrigEgammaEmulationTool.TrigEgammaEmulationPidToolsConfig     import clearAllAsgSelectors
  from TrigEgammaEmulationTool.TrigEgammaEmulationPidToolsConfig     import ElectronPidTools 
  from TrigEgammaEmulationTool.TrigEgammaEmulationPidToolsConfig     import PhotonPidTools
  clearAllAsgSelectors()
  # Set all Pid tools using the emulator configuration parameters
  print 'Adding New Likelihood tuning, 201607'
  ElectronPidTools()
  PhotonPidTools()



from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationTool
from TrigEgammaEmulationTool.TrigEgammaEmulationEFConfig   import EgammaEFElectronDefaultEmulator,EgammaEFElectronNoD0Emulator

EmulationElectronTool = TrigEgammaEmulationTool.copy( name="TrigEgammaEmulationTool",
                                                      TriggerList = triggerList,
                                                      SupportingTriggerList = probeListLowMidPtSupportingTriggers+probeListHighPtSupportingTriggers,
                                                      EFElectronSelectorTools = [EgammaEFElectronDefaultEmulator,
                                                                                 EgammaEFElectronNoD0Emulator],
                                                      OutputLevel=0)

####################################################################################################

AnalysisTool = TrigEgammaNavTPAnalysisTool( name = "TrigEgammaNavTPAnalysisTool",
                                            Analysis='Zee',
                                            ElectronKey = 'Electrons',
                                            PlotTool=PlotTool,
                                            Tools=[EffTool],
                                            isEMResultNames=["Tight","Medium","Loose"],
                                            ZeeLowerMass=80,
                                            ZeeUpperMass=100,
                                            OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose
                                            OfflineProbeSelector='Loose', 
                                            ForceProbePid=False, 
                                            OppositeCharge=True,
                                            doJpsiee=False,
                                            doEmulation=True,
                                            DetailedHistograms=True,
                                            EmulationTool=EmulationElectronTool,
                                            TPTrigger=False,
                                            OfflineTagMinEt=25,
                                            RemoveCrack=True,
                                            TagTriggerList=triggerTags,
                                            TriggerList=triggerList,
                                            OutputLevel=0)

####################################################################################################
Tools=['TrigEgammaNavTPAnalysisTool/TrigEgammaNavTPAnalysisTool']
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
TrigEgammaMonTool = TrigEgammaMonTool( name = "HLTEgammaMon", histoPathBase=basePath, Tools=Tools)
#ToolSvc += TrigEgammaMonTool
HLTMonManager.AthenaMonTools += [ TrigEgammaMonTool ]
HLTMonManager.FileKey = "GLOBAL"
#ToolSvc.TrigDecisionTool.Navigation.OutputLevel = WARNING
####################################################################################################
