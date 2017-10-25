# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaAnalysisTools import TrigEgammaAnalysisToolsConf
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc

from egammaRec.Factories import ToolFactory,FcnWrapper,AlgFactory, getPropertyValue 

import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
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

# Offline selectors -- taken from latest conf
LooseElectronSelector             = CfgMgr.AsgElectronIsEMSelector("T0HLTLooseElectronSelector")
MediumElectronSelector            = CfgMgr.AsgElectronIsEMSelector("T0HLTMediumElectronSelector")
TightElectronSelector             = CfgMgr.AsgElectronIsEMSelector("T0HLTTightElectronSelector")
LooseLHSelector                   = CfgMgr.AsgElectronLikelihoodTool("T0HLTLooseLHSelector")
MediumLHSelector                  = CfgMgr.AsgElectronLikelihoodTool("T0HLTMediumLHSelector")
TightLHSelector                   = CfgMgr.AsgElectronLikelihoodTool("T0HLTTightLHSelector")
LooseElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"
MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"
TightElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"
LooseLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodLooseOfflineConfig2017_CutBL_Smooth.conf"
MediumLHSelector.ConfigFile       = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodMediumOfflineConfig2017_Smooth.conf"
TightLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodTightOfflineConfig2017_Smooth.conf"
ToolSvc+=LooseElectronSelector
ToolSvc+=MediumElectronSelector
ToolSvc+=TightElectronSelector
ToolSvc+=LooseLHSelector
ToolSvc+=MediumLHSelector
ToolSvc+=TightLHSelector


#from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
#lumiTool = LuminosityToolOnline()
#ToolSvc += lumiTool 
#from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
#lumiTool = LuminosityToolDefault()
#ToolSvc += lumiTool
from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
lumiOnlineTool = LuminosityToolOnline("OnlLuminosityTool")
if not hasattr(ToolSvc,lumiOnlineTool.getName()):
    ToolSvc += lumiOnlineTool

IneffLabels=["ClusterEtaRange","ConversionMatch","ClusterHadronicLeakage","ClusterMiddleEnergy","ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth","f3","ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE","ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c","empty14","ClusterStripsDEmaxs1","TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","TrackTRTeProbabilityHT_Electron","TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight","Isolation","ClusterIsolation","TrackIsolation","No Track","No Cluster","No Object"]

from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoring_mam, monitoring_electron, monitoring_photon 
from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoringTP_electron, monitoringTP_electronZee, monitoringTP_electronJpsiee 

from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool

EgammaMatchTool = Trig__TrigEgammaMatchingTool()
ToolSvc += EgammaMatchTool

# Plot Tool sets the base path for histograms of all tools
TrigEgammaPlotTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaPlotTool, name="TrigEgammaPlotTool",
        DirectoryPath="/HLT/Egamma",
        MaM=monitoring_mam,
        Efficiency=["eff_et","eff_eta","eff_mu"],
        Distribution=["et","eta"],
        Resolution=["res_et","res_eta"],
        OutputLevel=0)
EfficiencyTool = ToolFactory(TrigEgammaAnalysisToolsConf.EfficiencyTool, 
        name="EfficiencyTool",
        PlotTool=TrigEgammaPlotTool,
        isEMResultNames=["Tight","Medium","Loose"],
        LHResultNames=["LHTight","LHMedium","LHLoose"],
        OutputLevel=0)
ResolutionTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaResolutionTool, 
        name="ResolutionTool",
        PlotTool=TrigEgammaPlotTool,
        OutputLevel=0)
DistTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaDistTool, 
        name="DistTool",
        PlotTool=TrigEgammaPlotTool,
        OutputLevel=0)

# import emulator egamma trigger tool
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationTool
# Copy the default config w/ list
# Allows factories below to instantiate emulator once Factories are instantiated
EmulationTool = TrigEgammaEmulationTool.copy( name = "TrigEgammaEmulationTool",
                                         TriggerList = monitoringTP_electron)

TrigEgammaNavTPNtuple = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavTPNtuple, name ="TrigEgammaNavTPNtuple",
        Analysis='ZeeTPNtuple',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        PlotTool=TrigEgammaPlotTool,
        EmulationTool=EmulationTool,
        Tools=[],
        isEMResultNames=["Tight","Medium","Loose"],
        LHResultNames=["LHTight","LHMedium","LHLoose"],
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        ZeeLowerMass=80,
        ZeeUpperMass=100,
        doOfflineDump=False,
        doEmulation=False,
        OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose 
        OfflineProbeSelector='Loose', 
        ForceProbePid=False, 
        OppositeCharge=True,
        OfflineTagMinEt=25,
        CutLabels=["Events","LAr","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"],
        TagTriggerList=["e24_tight_iloose"],
        TriggerList=monitoringTP_electron

        )

TrigEgammaNavNtuple = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavNtuple, name ="TrigEgammaNavNtuple",
        Analysis='Ntuple',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        PlotTool=TrigEgammaPlotTool,
        EmulationTool=EmulationTool,
        Tools=[],
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        TriggerList=monitoringTP_electron,
        doOfflineDump=False,
        doEmulation=False,
        ForcePidSelection=True,
        ForceProbeIsolation=False,
        ForceEtThreshold=True,
        ForceMCEnhancedBias=False,
        RemoveCrack=True,
        )

TrigEgammaNavTPAnalysisTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavTPAnalysisTool, name = "TrigEgammaNavTPAnalysisTool",
        Analysis='Zee',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        PlotTool=TrigEgammaPlotTool,
        #EmulationTool=EmulationTool,
        Tools=[EfficiencyTool,ResolutionTool,DistTool],
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
        doEmulation=False,
        TPTrigger=False,
        OfflineTagMinEt=25,
        RemoveCrack=True,
        TagTriggerList=["e26_tight_iloose"],
        TriggerList=monitoringTP_electron,
        IsEMLabels=IneffLabels,
        CutLabels=["Events","LAr","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"],
        ProbeLabels=["Electrons","NotTag","OS","SS","ZMass","HasTrack","HasCluster","Eta","Et","IsGoodOQ","GoodPid","NearbyJet","Isolated",],
        TagLabels=["Electrons","HasTrack","HasCluster","GoodPid","Et","Eta","IsGoodOQ","PassTrigger","MatchTrigger"],
        )

TrigEgammaNavTPJpsieeAnalysisTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavTPAnalysisTool,
        name = "TrigEgammaNavTPJpsieeAnalysisTool",
        Analysis='Jpsiee',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        PlotTool=TrigEgammaPlotTool,
        #EmulationTool=EmulationTool,
        Tools=[EfficiencyTool],
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
        doEmulation=False,
        TPTrigger=False,
        OfflineTagMinEt=5,
        RemoveCrack=True,
        TagTriggerList=["e5_tight"],
        TriggerList=monitoringTP_electronJpsiee,
        IsEMLabels=IneffLabels,
        CutLabels=["Events","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"],
        ProbeLabels=["Electrons","NotTag","OS","SS","ZMass","HasTrack","HasCluster","Eta","Et","GoodPid","NearbyJet","Isolated",],
        TagLabels=["Electrons","HasTrack","HasCluster","GoodPid","Et","Eta","PassTrigger","MatchTrigger"],
        )

TrigEgammaNavAnalysisTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavAnalysisTool, name ="TrigEgammaNavAnalysisTool",
        Analysis='Analysis',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        PlotTool=TrigEgammaPlotTool,
        #EmulationTool=EmulationTool,
        Tools=[EfficiencyTool,ResolutionTool,DistTool],
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        IsEMLabels=IneffLabels,
        TriggerList=monitoring_electron + monitoring_photon,
        ForcePidSelection=True,
        ForceProbeIsolation=False,
        ForceEtThreshold=True,
        RemoveCrack=True,
        doEmulation=False,
        )

TrigEgammaNavEnhancedBiasAnalysisTool = ToolFactory(TrigEgammaAnalysisToolsConf.TrigEgammaNavAnalysisTool, name ="TrigEgammaNavEnhancedBiasAnalysisTool",
        Analysis='EnhancedBias',
        ElectronKey = 'Electrons',
        MatchTool = EgammaMatchTool,
        EmulationTool=EmulationTool,
        PlotTool=TrigEgammaPlotTool,
        Tools=[EfficiencyTool,ResolutionTool,DistTool],
        ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
        ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector], 
        IsEMLabels=IneffLabels,
        TriggerList=monitoring_electron + monitoring_photon,
        ForcePidSelection=False,
        ForceProbeIsolation=False,
        ForceEtThreshold=False,
        ForceMCEnhancedBias=False,
        RemoveCrack=False,
        doEmulation=False,
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
