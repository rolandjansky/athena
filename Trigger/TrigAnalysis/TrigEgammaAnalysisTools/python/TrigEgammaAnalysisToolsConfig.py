# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Create dictionary for MaM
TrigEgammaMaMDict = {
        'primary_single_ele':['e24_lhmedium_L1EM18VH'],
        'primary_single_ele_cutbased':['e24_medium_L1EM18VH'],
        'primary_double_ele':['2e12_lhloose_L12EM10VH'],
        'primary_double_ele_cutbased':['2e12_loose_L12EM10VH'],
        'monitoring_ele_idperf':['e24_lhmedium_idperf_L1EM20VH'],
        'monitoring_ele_idperf_cutbased':['e24_medium_idperf_L1EM20VH'],
        'monitoring_Zee':['e24_lhtight_L1EM20VH_e15_etcut_Zee'],
        'monitoring_Jpsiee':['e5_lhtight_e4_etcut_Jpsiee'],
        'primary_single_pho':['g120_loose'],
        'primary_double_pho':['g35_loose_g25_loose'] }
  
from TrigEgammaAnalysisTools import TrigEgammaAnalysisToolsConf
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc

from egammaRec.Factories import ToolFactory,FcnWrapper,AlgFactory, getPropertyValue 
from egammaMVACalib.egammaMVACalibConf import egammaMVATool
mvatool = egammaMVATool("mvatool",folder="egammaMVACalib/v1")
ToolSvc += mvatool

import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum
from ROOT import egammaPID

# Following loads the online selectors
from TrigEgammaHypo.TrigEgammaPidTools import ElectronPidTools
from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
ElectronPidTools()
PhotonPidTools()

from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronIsEMSelector
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import ElectronIsEMMap,electronPIDmenu

# Offline ++ selectors -- taken from release
# Can be removed after comparison with r6630
#from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
#LooseElectronSelector = ConfiguredAsgElectronIsEMSelector("LooseElectronSelector", egammaPID.ElectronIDLoosePP)
#ToolSvc+=LooseElectronSelector
#MediumElectronSelector = ConfiguredAsgElectronIsEMSelector("MediumElectronSelector", egammaPID.ElectronIDMediumPP)
#ToolSvc+=MediumElectronSelector
#TightElectronSelector = ConfiguredAsgElectronIsEMSelector("TightElectronSelector", egammaPID.ElectronIDTightPP)
#ToolSvc+=TightElectronSelector
# Likelihood
#from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
#LooseLHSelector = ConfiguredAsgElectronLikelihoodTool("LooseLHSelector", LikeEnum.Loose)
#LooseLHSelector.primaryVertexContainer="PrimaryVertices" 
#ToolSvc+=LooseLHSelector
#MediumLHSelector = ConfiguredAsgElectronLikelihoodTool("MediumLHSelector", LikeEnum.Medium)
#MediumLHSelector.primaryVertexContainer="PrimaryVertices"
#ToolSvc+=MediumLHSelector
#TightLHSelector = ConfiguredAsgElectronLikelihoodTool("TightLHSelector", LikeEnum.Tight)
#TightLHSelector.primaryVertexContainer="PrimaryVertices"
#ToolSvc+=TightLHSelector

# Offline selectors -- taken from latest conf
LooseElectronSelector=CfgMgr.AsgElectronIsEMSelector("LooseElectronSelector")
LooseElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"
ToolSvc+=LooseElectronSelector
MediumElectronSelector=CfgMgr.AsgElectronIsEMSelector("MediumElectronSelector")
MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"
ToolSvc+=MediumElectronSelector
TightElectronSelector=CfgMgr.AsgElectronIsEMSelector("TightElectronSelector")
TightElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"
ToolSvc+=TightElectronSelector

LooseLHSelector=CfgMgr.AsgElectronLikelihoodTool("LooseLHSelector")
LooseLHSelector.ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodLooseOfflineConfig2015.conf"
ToolSvc+=LooseLHSelector
MediumLHSelector=CfgMgr.AsgElectronLikelihoodTool("MediumLHSelector")
MediumLHSelector.ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodMediumOfflineConfig2015.conf"
ToolSvc+=MediumLHSelector
TightLHSelector=CfgMgr.AsgElectronLikelihoodTool("TightLHSelector")
TightLHSelector.ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronLikelihoodTightOfflineConfig2015.conf"
ToolSvc+=TightLHSelector

#from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
#lumiTool = LuminosityToolOnline()
#ToolSvc += lumiTool 
#from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
#lumiTool = LuminosityToolDefault()
#ToolSvc += lumiTool
#from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
#lumiOnlineTool = LuminosityToolOnline("LuminosityToolOnline")
#ToolSvc += lumiOnlineTool

IneffLabels=["ClusterEtaRange","ConversionMatch","ClusterHadronicLeakage","ClusterMiddleEnergy","ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth","f3","ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE","ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c","empty14","ClusterStripsDEmaxs1","TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","TrackTRTeProbabilityHT_Electron","TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight","Isolation","ClusterIsolation","TrackIsolation","No Track","No Cluster","No Object"]

from TrigEgammaAnalysisTools.TrigEgammaProbelist import * # to import probelist
triggerlist = default
#EgammaMatchTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaMatchingTool,name="TrigEgammaMatchingTool",DeltaR=0.07,L1DeltaR=0.15)
from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool
EgammaMatchTool = Trig__TrigEgammaMatchingTool("MatchingTool")
ToolSvc += EgammaMatchTool
# Base tool configuration here as example
# All tools inherit these triggerlist properties
# These are triggerlist in constructor as well
# Using Factories need to set for each tool
EfficiencyTool = ToolFactory(TrigEgammaAnalysisToolsConf.EfficiencyTool, 
        name="EfficiencyTool",
        isEMResultNames=["Tight","Medium","Loose"],
        LHResultNames=["LHTight","LHMedium","LHLoose"],
        OutputLevel=2)
ResolutionTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaResolutionTool, name="ResolutionTool",OutputLevel=0)
DistTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaDistTool, name="DistTool",OutputLevel=0)
PlotTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaPlotTool, name="TrigEgammaPlotTool",OutputLevel=0)


TrigEgammaNavTPNtuple = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavTPNtuple, name ="TrigEgammaNavTPNtuple",
        DirectoryPath='NavZeeTPNtuple',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        Tools=[EfficiencyTool],
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        isEMResultNames=["Tight","Medium","Loose"],
        LHResultNames=["LHTight","LHMedium","LHLoose"],
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        CutLabels=["Events","LAr","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"],
        TagTriggerList="e24_tight_iloose",
        TriggerList=triggerlist,
        )

TrigEgammaNavNtuple = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavNtuple, name ="TrigEgammaNavNtuple",
        DirectoryPath='NavNtuple',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        Tools=[EfficiencyTool],
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        TriggerList=triggerlist,
        OfflineDirectoryPath='Offline/Egamma/Ntuple', 
        DoOfflineDump=False,
        dR=0.07,
        ForcePidSelection=True,
        ForceProbeIsolation=False,
        ForceEtThreshold=True,
        RemoveCrack=True,
        ForceFilterSelection=False,
        ElectronFilterType="Tight",
        PhotonFilterType="Loose",
        )

TrigEgammaNavTPAnalysisTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavTPAnalysisTool, name = "TrigEgammaNavTPAnalysisTool",
        DirectoryPath='NavTPAnalysis',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        Tools=[EfficiencyTool,ResolutionTool,DistTool],
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        isEMResultNames=["Tight","Medium","Loose"],
        LHResultNames=["LHTight","LHMedium","LHLoose"],
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        doJpsiee=False,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        RemoveCrack=True,
        TagTriggerList=["e26_tight_iloose"],
        TriggerList=triggerlist,
        IsEMLabels=IneffLabels,
        CutLabels=["Events","LAr","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"],
        ProbeLabels=["Electrons","NotTag","OS","SS","ZMass","HasTrack","HasCluster","Eta","Et","IsGoodOQ","GoodPid","NearbyJet","Isolated",],
        TagLabels=["Electrons","HasTrack","HasCluster","GoodPid","Et","Eta","IsGoodOQ","PassTrigger","MatchTrigger"],
        )

TrigEgammaNavTPJpsieeAnalysisTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavTPAnalysisTool,
         name = "TrigEgammaNavTPJpsieeAnalysisTool",
         DirectoryPath='NavTPJpsieeAnalysis',
         ElectronKey = 'Electrons',
         MatchTool = EgammaMatchTool,
         Tools=[EfficiencyTool],
         MVACalibTool=mvatool,
         ApplyMVACalib=False,
         isEMResultNames=["Tight","Medium","Loose"],
         LHResultNames=["LHTight","LHMedium","LHLoose"],
         ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
         ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
         ZeeLowerMass=2,
         ZeeUpperMass=5,
         OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
         OfflineProbeSelector='Loose', 
         ForceProbePid=False, 
         OppositeCharge=True,
         doJpsiee=True,
         OfflineTagMinEt=5,
         OfflineProbeMinEt=5,
         RemoveCrack=True,
         TagTriggerList=["e5_tight"],
         TriggerList=defaultJpsi,
         IsEMLabels=IneffLabels,
         CutLabels=["Events","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"],
         ProbeLabels=["Electrons","NotTag","OS","SS","ZMass","HasTrack","HasCluster","Eta","Et","GoodPid","NearbyJet","Isolated",],
         TagLabels=["Electrons","HasTrack","HasCluster","GoodPid","Et","Eta","PassTrigger","MatchTrigger"],
         )

TrigEgammaNavAnalysisTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavAnalysisTool, name ="TrigEgammaNavAnalysisTool",
        DirectoryPath='NavAnalysis',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        Tools=[EfficiencyTool,ResolutionTool,DistTool],
        MVACalibTool=mvatool,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        IsEMLabels=IneffLabels,
        TriggerList=triggerlist,
        dR = 0.07,
        ForcePidSelection=True,
        ForceProbeIsolation=False,
        ForceEtThreshold=True,
        RemoveCrack=True,
        ForceFilterSelection=False,
        ElectronFilterType="Tight",
        PhotonFilterType="Loose", 
        )


TrigEgammaEmulationTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaEmulationTool, name = "TrigEgammaEmulationTool",
        DirectoryPath='Emulation',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        Tools=[EfficiencyTool],
        TriggerList=triggerlist,
        PhotonOnlPPSelector=[ ToolSvc.AsgPhotonIsEMTightSelector,
            ToolSvc.AsgPhotonIsEMMediumSelector,
            ToolSvc.AsgPhotonIsEMLooseSelector],
        ElectronOnlPPSelector=[ ToolSvc.AsgElectronIsEMTightHypoSelector,
            ToolSvc.AsgElectronIsEMMediumHypoSelector,
            ToolSvc.AsgElectronIsEMLooseHypoSelector,
            ToolSvc.AsgElectronIsEMVLooseHypoSelector ],
        ElectronOnlLHSelector=[ ToolSvc.AsgElectronLHTightSelector,
            ToolSvc.AsgElectronLHMediumSelector,
            ToolSvc.AsgElectronLHLooseSelector,
            ToolSvc.AsgElectronLHVLooseSelector ],
        PhotonCaloPPSelector=[ ToolSvc.AsgPhotonIsEMTightSelector,
            ToolSvc.AsgPhotonIsEMMediumSelector,
            ToolSvc.AsgPhotonIsEMLooseSelector],
        ElectronCaloPPSelector=[ ToolSvc.AsgElectronIsEMTightCaloHypoSelector,
            ToolSvc.AsgElectronIsEMMediumCaloHypoSelector,
            ToolSvc.AsgElectronIsEMLooseCaloHypoSelector,
            ToolSvc.AsgElectronIsEMVLooseCaloHypoSelector ],
        ElectronCaloLHSelector=[ ToolSvc.AsgElectronLHTightCaloSelector,
            ToolSvc.AsgElectronLHMediumCaloSelector,
            ToolSvc.AsgElectronLHLooseCaloSelector,
            ToolSvc.AsgElectronLHVLooseCaloSelector ],
        )

# Function to return triggerlist tools
def getAllTools():
    return [TrigEgammaNavZeeTPCounts(),TrigEgammaNavZeeTPEff(),TrigEgammaNavZeeTPIneff(),TrigEgammaNavZeeTPRes(),]

# The main algorithm
# Add triggerlist tools to ToolHandleArray 
TrigEgammaAnalysisAlg = AlgFactory(TrigEgammaAnalysisToolsConf.TrigEgammaAnalysisAlg, 
        name='TrigEgammaAnalysisAlg',
        Tools = FcnWrapper(getAllTools),
        )
