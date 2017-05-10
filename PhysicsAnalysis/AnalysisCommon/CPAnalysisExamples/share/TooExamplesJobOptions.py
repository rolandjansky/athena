
#====================================================
#Imports - EXPERTS ONLY
#-------------------------
import ROOT
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import AthenaRootComps.ReadAthenaxAODHybrid                             #fast xAODReading
#import AthenaPoolCnvSvc.ReadAthenaPool                                 #alternative but more robust POOL reading
from PyUtils import AthFile                                             #file metadata access in joboptions
#====================================================


#====================================================
#Basic Job Configuration
#-------------------------
algseq = CfgMgr.AthSequencer("AthAlgSeq")                               #gets the main AthSequencer
algseq += CfgMgr.ToolExamplesAlg("MyAlg")                               #Add Algorithm to sequence
#====================================================


#====================================================
#Basic Job Settings
#-------------------------

athenaCommonFlags.EvtMax = 10                                           #number of events to process
athenaCommonFlags.FilesInput = [os.environ['ASG_TEST_FILE_MC']]         #list of input files

svcMgr.EventSelector.AccessMode = 2                                     #0 = branch, 1 = class, 2 = athena
#====================================================



#====================================================
#Metadata extraction - EXPERTS ONLY
#-------------------------
#this fills a dictionary with useful metadata about the input file - used in configurations below
#see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_at_t
metadata = dict()
af = AthFile.fopen(athenaCommonFlags.FilesInput()[0])                     #extra metadata from first input file
metadata['streamName'] = af.fileinfos['stream_names']
metadata['isMC'] = ('IS_SIMULATION' in af.fileinfos['evt_type'])
metadata['conditionsTag'] = af.fileinfos['conditions_tag']
metadata['isFastSim'] = metadata['isMC'] and ('ATLFASTII' in af.fileinfos['metadata']['/Simulation/Parameters']['SimulationFlavour'].upper())
metadata['datasetNumber'] = af.mc_channel_numbers[0]
#====================================================





#====================================================
#Tool Configurations
#-------------------------

#Notes:
#  Any property listed here is one should ensure is set correctly


#-----------------------
## GENERAL 
#-----------------------

#GRL Selection
#Twiki:
ToolSvc += CfgMgr.GoodRunsListSelectionTool("GRLTool")
ToolSvc.GRLTool.GoodRunsListVec=["dev/GoodRunsLists/data15_13TeV/latest/physics_25ns_20.7.xml", #replace with your GRL
                                 "dev/GoodRunsLists/data16_13TeV/latest/physics_25ns_20.7.xml"]


#Pileup Reweighting
#Twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting
ToolSvc += CfgMgr.CP__PileupReweightingTool("PileupReweightingTool")
ToolSvc.PileupReweightingTool.ConfigFiles=["dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"]   #replace with your PRW config files
ToolSvc.PileupReweightingTool.LumiCalcFiles=["dev/GoodRunsLists/data15_13TeV/latest/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root", #replace with your lumicalc files
                                             "dev/GoodRunsLists/data16_13TeV/latest/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root"]
ToolSvc.PileupReweightingTool.DataScaleFactor=1./1.16       #see twiki
ToolSvc.PileupReweightingTool.DataScaleFactorUP=1.          #see twiki
ToolSvc.PileupReweightingTool.DataScaleFactorDOWN=1./1.23   #see twiki


#Trigger Decision
#Twiki: 
ToolSvc += CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool")

#Trigger Matching
#Twiki: https://twiki.cern.ch/twiki/bin/view/Atlas/XAODMatchingTool
ToolSvc += CfgMgr.Trig__MatchingTool("TrigMatchingTool")
ToolSvc.TrigMatchingTool.TrigDecisionTool = ToolSvc.TrigDecisionTool



#-----------------------
## ELECTRONS 
#-----------------------
#Calibration
#Twiki: 
ToolSvc += CfgMgr.CP__EgammaCalibrationAndSmearingTool("EgammaCalibTool")
ToolSvc.EgammaCalibTool.ESModel="es2016PRE"
ToolSvc.EgammaCalibTool.decorrelationModel="1NP_v1"
ToolSvc.EgammaCalibTool.useAFII=metadata['isFastSim']

#Isolation Calibration
#Twiki: 
ToolSvc += CfgMgr.CP__IsolationCorrectionTool("IsolationCorrectionTool")
ToolSvc.IsolationCorrectionTool.IsMC=metadata['isMC']
ToolSvc.IsolationCorrectionTool.AFII_corr=ToolSvc.EgammaCalibTool.useAFII

#Selection
#Twiki: 
ToolSvc += CfgMgr.AsgElectronLikelihoodTool("ElectronSelectionTool")
ToolSvc.ElectronSelectionTool.ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_CutBL_Smooth.conf"
#loose is "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_CutBL_Smooth.conf"
#medium is "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodMediumOfflineConfig2016_Smooth.conf"
#vloose is "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodVeryLooseOfflineConfig2016_Smooth.conf"

#Isolation Selection
#Twiki: 
ToolSvc += CfgMgr.CP__IsolationSelectionTool("ElectronIsolationTool")
ToolSvc.ElectronIsolationTool.ElectronWP = "Loose"                                              #Loose, Tight, FixedCutLoose, FixedCutTight, LooseTrackOnly, TightTrackOnly, Gradient

#Isolation CloseBy Correction
#Twiki: 
ToolSvc += CfgMgr.CP__IsolationCloseByCorrectionTool("ElectronIsolationCloseByCorrectionTool")
ToolSvc.ElectronIsolationCloseByCorrectionTool.IsolationSelectionTool=ToolSvc.ElectronIsolationTool

#ScaleFactors need the following 'code'
dataTypeCode = 0 #data
if metadata['isMC']:
  if metadata['isFastSim']: dataTypeCode=3 #fastsim
  else: dataTypeCode = 1 #fullsim

#Reco ScaleFactor
#Twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ElectronEfficiencyCorrelationModel
ToolSvc += CfgMgr.AsgElectronEfficiencyCorrectionTool("ElectronRecoSF")
ToolSvc.ElectronRecoSF.ForceDataType=dataTypeCode
ToolSvc.ElectronRecoSF.CorrelationModel="TOTAL"                      #FULL (~240 systematics), SIMPLIFIED (~25 systematics), TOTAL (1 systematic), COMBMCTOYS (see ElectronEffcieincyCorrelationModel)
#ToolSvc.ElectronRecoSF.NumberOfToys=40  -- set this if using COMBMCTOYS
ToolSvc.ElectronRecoSF.CorrectionFileNameList=["ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v3/offline/efficiencySF.offline.RecoTrk.root"]


#ID ScaleFactor
#Twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ElectronEfficiencyCorrelationModel
ToolSvc += CfgMgr.AsgElectronEfficiencyCorrectionTool("ElectronIDSF")
ToolSvc.ElectronIDSF.ForceDataType=dataTypeCode
ToolSvc.ElectronIDSF.CorrelationModel="TOTAL"
ToolSvc.ElectronIDSF.CorrectionFileNameList=["ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v3/offline/efficiencySF.offline.LooseAndBLayerLLH_d0z0_v11.root"]
#loose = ["ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v3/offline/efficiencySF.offline.LooseAndBLayerLLH_d0z0_v11.root"]
#medium = ??
#vloose = ??


#Isolation ScaleFactor
#Twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ElectronEfficiencyCorrelationModel
ToolSvc += CfgMgr.AsgElectronEfficiencyCorrectionTool("ElectronIsolSF")
ToolSvc.ElectronIsolSF.ForceDataType=dataTypeCode
ToolSvc.ElectronIsolSF.CorrelationModel="TOTAL"
ToolSvc.ElectronIsolSF.CorrectionFileNameList=["ElectronEfficiencyCorrection/2015_2016/rel20.7/ICHEP_June2016_v3/isolation/efficiencySF.Isolation.LooseAndBLayerLLH_d0z0_v11_isol" + ToolSvc.ElectronIsolationTool.ElectronWP + ".root"]



#-----------------------
## MUONS 
#-----------------------
#Calibration
#Twiki: 
ToolSvc += CfgMgr.CP__MuonCalibrationAndSmearingTool("MuonCalibTool")

#Selection
#Twiki: 
ToolSvc += CfgMgr.CP__MuonSelectionTool("MuonSelectionTool")
ToolSvc.MuonSelectionTool.MaxEta = 2.7
ToolSvc.MuonSelectionTool.MuQuality = 2                                                 #tight = 0, medium = 1, loose = 2

#Isolation Selection
#Twiki: 
ToolSvc += CfgMgr.CP__IsolationSelectionTool("MuonIsolationTool")
ToolSvc.MuonIsolationTool.MuonWP = "Loose"                                              #Loose, FixedCutLoose, LooseTrackOnly, Gradient

#Reco ScaleFactor\
#Twiki: 
ToolSvc += CfgMgr.CP__MuonEfficiencyScaleFactors("MuonRecoSF",PileupReweightingTool=ToolSvc.PileupReweightingTool)
if   ToolSvc.MuonSelectionTool.MuQuality==0:  ToolSvc.MuonRecoSF.WorkingPoint ="Tight"
elif ToolSvc.MuonSelectionTool.MuQuality==1:  ToolSvc.MuonRecoSF.WorkingPoint ="Medium"
elif ToolSvc.MuonSelectionTool.MuQuality==2:  ToolSvc.MuonRecoSF.WorkingPoint ="Loose"

#TrackToVertexAssociation (TTVA) ScaleFactor - to be applied if we do the d0 and z0 cuts
#Twiki: 
ToolSvc += CfgMgr.CP__MuonEfficiencyScaleFactors("MuonTTVASF",PileupReweightingTool=ToolSvc.PileupReweightingTool)
ToolSvc.MuonTTVASF.WorkingPoint="TTVA"

#Isolation ScaleFactor
#Twiki: 
ToolSvc += CfgMgr.CP__MuonEfficiencyScaleFactors("MuonIsolSF",PileupReweightingTool=ToolSvc.PileupReweightingTool)
ToolSvc.MuonIsolSF.WorkingPoint=ToolSvc.MuonIsolationTool.MuonWP

#Trigger ScaleFactor
#Twiki: 
#???

#----------------------
## JETS
#----------------------

#Jet Calibration
#Twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ApplyJetCalibration2016
ToolSvc += CfgMgr.JetCalibrationTool("JetCalibTool")
ToolSvc.JetCalibTool.IsData=not metadata['isMC']
if metadata['isFastSim']: ToolSvc.JetCalibTool.ConfigFile = "JES_MC15Prerecommendation_AFII_June2015.config"
else:                     ToolSvc.JetCalibTool.ConfigFile="JES_MC15cRecommendation_May2016.config"
ToolSvc.JetCalibTool.CalibSequence="JetArea_Residual_Origin_EtaJES_GSC"
ToolSvc.JetCalibTool.JetCollection="AntiKt4EMTopo"

#Jet Cleaning
#Twiki: 
ToolSvc += CfgMgr.JetCleaningTool("JetCleaningTool")
ToolSvc.JetCleaningTool.CutLevel="LooseBad"
ToolSvc.JetCleaningTool.DoUgly=False


#JES Uncertainties
#Twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetUncertainties
#???

#JER Uncertainties
#Twiki: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ApplyJetResolutionSmearing
#???


#====================================================


#====================================================
#EXPERTS ONLY
#-------------------------

#transfer properties to core components
theApp.EvtMax=athenaCommonFlags.EvtMax()
svcMgr.EventSelector.InputCollections=athenaCommonFlags.FilesInput()

include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above
#====================================================
