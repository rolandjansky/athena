# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


def TrigEgammaAnalysisToolsCfg( runFlag ):

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()


    from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool
    EgammaMatchTool = Trig__TrigEgammaMatchingTool()
    EgammaMatchTool.OutputLevel=2
    EgammaMatchTool.DeltaR=0.4

    acc.addPublicTool(EgammaMatchTool)


    from TrigEgammaAnalysisTools.TrigEgammaProbelist import monitoring_electron, monitoring_photon, monitoringTP_electronJpsiee, monitoringTP_electron
    from TrigEgammaAnalysisTools import TrigEgammaAnalysisToolsConf
    from AthenaCommon import CfgMgr

    
    import cppyy
    cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
    # Following loads the online selectors
    from TrigEgammaHypo.TrigEgammaPidTools import ElectronPidTools
    from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools

    # setup all pid tools
    ElectronPidTools()
    PhotonPidTools()

    # Offline selectors -- taken from latest conf
    LooseElectronSelector             = CfgMgr.AsgElectronIsEMSelector("T0HLTLooseElectronSelector")
    MediumElectronSelector            = CfgMgr.AsgElectronIsEMSelector("T0HLTMediumElectronSelector")
    TightElectronSelector             = CfgMgr.AsgElectronIsEMSelector("T0HLTTightElectronSelector")
    LooseLHSelector                   = CfgMgr.AsgElectronLikelihoodTool("T0HLTLooseLHSelector")
    MediumLHSelector                  = CfgMgr.AsgElectronLikelihoodTool("T0HLTMediumLHSelector")
    TightLHSelector                   = CfgMgr.AsgElectronLikelihoodTool("T0HLTTightLHSelector")
    VeryLooseLHSelector               = CfgMgr.AsgElectronLikelihoodTool("T0HLTVeryLooseLHSelector")



    acc.addPublicTool(LooseElectronSelector)
    acc.addPublicTool(MediumElectronSelector)
    acc.addPublicTool(TightElectronSelector)
    acc.addPublicTool(LooseLHSelector)
    acc.addPublicTool(MediumLHSelector)
    acc.addPublicTool(TightLHSelector)
    acc.addPublicTool(VeryLooseLHSelector)

 
    if runFlag == '2018':
      # cut based
      LooseElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"
      MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"
      TightElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"
      # 2018 (vtest)
      LooseLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodLooseOfflineConfig2017_CutBL_Smooth.conf"
      MediumLHSelector.ConfigFile       = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodMediumOfflineConfig2017_Smooth.conf"
      TightLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodTightOfflineConfig2017_Smooth.conf"
      VeryLooseLHSelector.ConfigFile    = "ElectronPhotonSelectorTools/offline/mc16_20170828/ElectronLikelihoodVeryLooseOfflineConfig2017_Smooth.conf"
    elif runFlag == '2017':
      # cut based
      LooseElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMLooseSelectorCutDefs.conf"
      MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMMediumSelectorCutDefs.conf"
      TightElectronSelector.ConfigFile  = "ElectronPhotonSelectorTools/offline/mc15_20150712/ElectronIsEMTightSelectorCutDefs.conf"
      # 2017 (v11)
      LooseLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_CutBL_Smooth.conf"
      MediumLHSelector.ConfigFile       = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodMediumOfflineConfig2016_Smooth.conf"
      TightLHSelector.ConfigFile        = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodTightOfflineConfig2016_Smooth.conf"
      VeryLooseLHSelector.ConfigFile    = "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodVeryLooseOfflineConfig2016_Smooth.conf"
    else:
      # raise since the configuration its not defined
      raise RuntimeError( 'Wrong run flag configuration' )



    Zee   = TrigEgammaAnalysisToolsConf.TrigEgammaNavTPAnalysisTool(
            # Base configuration
            name = "HLTEgammaTPAnalysis",
            Analysis='Zee',
            BasePath='HLT/Egamma',
            MatchTool = EgammaMatchTool,
            Tools=[],
            TPTrigger=False,
            ElectronKey = 'Electrons',
            isEMResultNames=["Tight","Medium","Loose"],
            LHResultNames=["LHTight","LHMedium","LHLoose"],
            ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
            ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector],
            # Tag and probe configuration 
            ZeeLowerMass=80,
            ZeeUpperMass=100,
            OfflineTagMinEt=25,
            OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose
            OfflineProbeSelector='Loose',
            OppositeCharge=True,
            RemoveCrack=True,
            TagTriggerList=["e26_tight_iloose"],
            TriggerList=monitoringTP_electron,

            )
    
    
    
    Jpsiee =TrigEgammaAnalysisToolsConf.TrigEgammaNavTPAnalysisTool(
            # Base configuration
            name = "HLTEgammaTPJpsieeAnalysis",
            Analysis='Jpsiee',
            BasePath='HLT/Egamma',
            TPTrigger=False,
            ElectronKey = 'Electrons',
            MatchTool = EgammaMatchTool,
            Tools=[],
            isEMResultNames=["Tight","Medium","Loose"],
            LHResultNames=["LHTight","LHMedium","LHLoose"],
            ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
            ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector],
            # Tag and Probe configuration 
            ZeeLowerMass=2,
            ZeeUpperMass=5,
            OfflineTagMinEt=5,
            OfflineTagSelector='Tight', # 1=tight, 2=medium, 3=loose
            OfflineProbeSelector='Loose',
            OppositeCharge=True,
            DoJpsiee=True,
            RemoveCrack=True,
            TagTriggerList=["e5_tight"],
            TriggerList=monitoringTP_electronJpsiee,
            )
    
    
    Electron = TrigEgammaAnalysisToolsConf.TrigEgammaNavAnalysisTool( 
            name ="HLTEgammaElectronAnalysis",
            Analysis='Electrons',
            BasePath='HLT/Egamma',
            ElectronKey = 'Electrons',
            MatchTool = EgammaMatchTool,
            Tools=[],
            ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
            ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector],
            ElectronLHVLooseTool=VeryLooseLHSelector,
            TriggerList=monitoring_electron + monitoring_photon,
            ForcePidSelection=True,
            ForceProbeIsolation=False,
            ForceEtThreshold=True,
            RemoveCrack=True,
            )


 
    Photons= TrigEgammaAnalysisToolsConf.TrigEgammaNavAnalysisTool( 
            name ="HLTEgammaPhotonAnalysis",
            Analysis='Photons',
            BasePath='HLT/Egamma',
            ElectronKey = 'Electrons',
            MatchTool = EgammaMatchTool,
            Tools=[],
            ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector],
            ElectronLikelihoodTool =[TightLHSelector,MediumLHSelector,LooseLHSelector],
            ElectronLHVLooseTool=VeryLooseLHSelector,
            TriggerList=monitoring_electron + monitoring_photon,
            ForcePidSelection=True,
            ForceProbeIsolation=False,
            ForceEtThreshold=True,
            RemoveCrack=True,
            )

    acc.addPublicTool( Zee )
    acc.addPublicTool( Jpsiee )
    acc.addPublicTool( Electron )
    acc.addPublicTool( Photons )


    return acc, {'Zee':Zee, 'Jpsiee':Jpsiee, 'Electron':Electron, 'Photon':Photons}


