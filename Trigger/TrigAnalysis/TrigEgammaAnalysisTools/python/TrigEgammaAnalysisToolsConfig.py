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
from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
ElectronPidTools()
PhotonPidTools()

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
        ElectronKey = 'Electrons',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        )

TrigEgammaNavZeeTPCounts = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPCounts, name = "TrigEgammaNavZeeTPCounts",
        OutputLevel=1,
        DirectoryPath='NavZeeTPCounts',
        ElectronKey = 'Electrons',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose','e24_medium_iloose','e28_lhtight_iloose'],
        dR = 0.07,
        )

TrigEgammaNavZeeTPEff = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPEff, name = "TrigEgammaNavZeeTPEff",
        DirectoryPath='NavZeeTPEff',
        ElectronKey = 'Electrons',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        #ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        dR = 0.07,
        )

TrigEgammaNavZeeTPRes = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPRes, name = "TrigEgammaNavZeeTPRes",
        DirectoryPath='NavZeeTPRes',
        ElectronKey = 'Electrons',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector], 
        #ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPP, # Set differently than base tool
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        dR = 0.07,
        )

TrigEgammaNavZeeTPIneff = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPIneff, name = "TrigEgammaNavZeeTPIneff",
        DirectoryPath='NavZeeTPIneff',
        ElectronKey = 'Electrons',
        IneffLabels=["ClusterEtaRange","ConversionMatch","ClusterHadronicLeakage","ClusterMiddleEnergy","ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth","f3","ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE","ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c","empty14","ClusterStripsDEmaxs1","TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","TrackTRTeProbabilityHT_Electron","TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight","Isolation","ClusterIsolation","TrackIsolation"],
        ElectronOnlPPSelector=[ ToolSvc.AsgElectronIsEMTightSelector,ToolSvc.AsgElectronIsEMMediumSelector,ToolSvc.AsgElectronIsEMLooseSelector ],
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        TagTrigger="e28_tight_iloose",
        ProbeTriggerList=['e28_tight_iloose'],
        dR = 0.07,
        )

TrigEgammaNavZeeTPPerf = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPPerf, name = "TrigEgammaNavZeeTPPerf",
        DirectoryPath='NavZeeTPPerf',
        ElectronKey = 'Electrons',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        TagTrigger="e24_tight_iloose",
        ProbeTriggerList=['e24_tight_iloose'],
        dR = 0.07,
        )

TrigEgammaNavZeeTPNtuple = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavZeeTPNtuple, name ="TrigEgammaNavZeeTPNtuple",
        DirectoryPath='NavZeeTPNtuple',
        ElectronKey = 'Electrons',
        MVACalibTool=mvatool,
        ApplyMVACalib=False,
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        OfflineTagIsEM=egammaPID.ElectronTightPPIso,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        OfflineProbeMinEt=24,
        TagTrigger="e24_tight_iloose",
        ProbeTriggerList=['e24_tight_iloose'],
        dR = 0.07,
        doRinger=False,
        )

TrigEgammaEmulationTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaEmulationTool, name = "TrigEgammaEmulationTool",
        DirectoryPath='Emulation',
        ElectronKey = 'Electrons',
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

# Function to return default tools
def getAllTools():
    return [TrigEgammaNavZeeTPCounts(),TrigEgammaNavZeeTPEff(),TrigEgammaNavZeeTPIneff(),TrigEgammaNavZeeTPRes(),]

# The main algorithm
# Add default tools to ToolHandleArray 
TrigEgammaAnalysisAlg = AlgFactory(TrigEgammaAnalysisToolsConf.TrigEgammaAnalysisAlg, 
        name='TrigEgammaAnalysisAlg',
        Tools = FcnWrapper(getAllTools),
        )
