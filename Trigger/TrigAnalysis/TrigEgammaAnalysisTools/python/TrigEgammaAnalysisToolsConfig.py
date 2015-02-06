# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaAnalysisTools import TrigEgammaAnalysisToolsConf
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
from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
ElectronPidTools()

from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu

# Offline ++ selectors
from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
LooseElectronSelector = ConfiguredAsgElectronIsEMSelector("LooseElectronSelector", egammaPID.ElectronIDLoosePP)
LooseElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronIsEMLooseSelectorCutDefs.conf"
ToolSvc+=LooseElectronSelector
MediumElectronSelector = ConfiguredAsgElectronIsEMSelector("MediumElectronSelector", egammaPID.ElectronIDMediumPP)
MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronIsEMMediumSelectorCutDefs.conf"
ToolSvc+=MediumElectronSelector
TightElectronSelector = ConfiguredAsgElectronIsEMSelector("TightElectronSelector", egammaPID.ElectronIDTightPP)
TightElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronIsEMTightSelectorCutDefs.conf"
ToolSvc+=TightElectronSelector
# Likelihood
from ElectronPhotonSelectorTools.ConfiguredAsgElectronLikelihoodTools import ConfiguredAsgElectronLikelihoodTool
LooseLHSelector = ConfiguredAsgElectronLikelihoodTool("LooseLHSelector", LikeEnum.Loose)
LooseLHSelector.primaryVertexContainer="PrimaryVertices" 
ToolSvc+=LooseLHSelector
MediumLHSelector = ConfiguredAsgElectronLikelihoodTool("MediumLHSelector", LikeEnum.Medium)
MediumLHSelector.primaryVertexContainer="PrimaryVertices"
ToolSvc+=MediumLHSelector
TightLHSelector = ConfiguredAsgElectronLikelihoodTool("TightLHSelector", LikeEnum.Tight)
TightLHSelector.primaryVertexContainer="PrimaryVertices"
ToolSvc+=TightLHSelector

# Base tool configuration here as example
# All tools inherit these default properties
# These are default in constructor as well
# Using Factories need to set for each tool
TrigEgammaNavZeeTPBaseTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPBaseTool, name = "TrigEgammaNavZeeTPBaseTool",
        DirectoryPath = 'NavZeeTPBase',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        MinimumTriggerList=["e28_tight","e28_tight_iloose"],
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        )

TrigEgammaNavZeeTPCounts = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPCounts, name = "TrigEgammaNavZeeTPCounts",
        OutputLevel=1,
        DirectoryPath='NavZeeTPCounts',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        MinimumTriggerList=["e28_tight","e28_tight_iloose"],
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose','e24_medium_iloose','e28_lhtight_iloose'],
        )

TrigEgammaNavZeeTPEff = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPEff, name = "TrigEgammaNavZeeTPEff",
        DirectoryPath='NavZeeTPEff',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        #ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        MinimumTriggerList=["e28_tight","e28_tight_iloose"],
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        )

TrigEgammaNavZeeTPRes = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPRes, name = "TrigEgammaNavZeeTPRes",
        DirectoryPath='NavZeeTPRes',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        #ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        MinimumTriggerList=["e28_tight","e28_tight_iloose"],
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPP, # Set differently than base tool
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        )

TrigEgammaNavZeeTPIneff = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPIneff, name = "TrigEgammaNavZeeTPIneff",
        DirectoryPath='NavZeeTPIneff',
        IneffLabels=["ClusterEtaRange","ConversionMatch","ClusterHadronicLeakage","ClusterMiddleEnergy","ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth","f3","ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE","ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c","empty14","ClusterStripsDEmaxs1","TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","empty23","TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight","Isolation","ClusterIsolation","TrackIsolation"],
        ElectronOnlPPSelector=[ ToolSvc.AsgElectronIsEMTightSelector,ToolSvc.AsgElectronIsEMMediumSelector,ToolSvc.AsgElectronIsEMLooseSelector ],
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        MinimumTriggerList=["e28_tight","e28_tight_iloose"],
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        )

# Function to return default tools
def getAllTools():
    return [TrigEgammaNavZeeTPCounts(),TrigEgammaNavZeeTPEff(),TrigEgammaNavZeeTPIneff(),TrigEgammaNavZeeTPRes(),]

# The main algorithm
# Add default tools to ToolHandleArray 
TrigEgammaAnalysisAlg = AlgFactory(TrigEgammaAnalysisToolsConf.TrigEgammaAnalysisAlg, 
        name='TrigEgammaAnalysisAlg',
        Tools = FcnWrapper(getAllTools),
        )
