# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to configure egammaAlgs to be used at the HLT" 
__author__ = "Fernando Monticelli"
"""
This file defines the factories of the algorithms to be used in a photon trigger sequence in athenaMT
These are inspired by the offline factories, alhtough modified so they reflect the configuration we need for these algorithms at the HLT. 
Offline configurations are available here:
    https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/


"""
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory,  FcnWrapper
from egammaMVACalib.egammaMVACalibFactories import egammaMVASvc
from egammaTools.egammaToolsFactories import egammaSwSuperClusterTool, EGammaAmbiguityTool
# Tools and funtions from TrigEgammaFactories
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigEMClusterTool, TrigEMShowerBuilder_HI, TrigEMShowerBuilder, TrigEgammaDecorationTools, TrigPhotonDecorationTools, TrigEMTrackMatchBuilder
# Load TrigEgammaKeys where we store the container names and other TrigEgamma configuration values
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaKeys import getTrigEgammaKeys


# Decoration tools for egamma and photon objects:
# Copying from https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaRec/python/topoEgammaGetter.py#L28

TrigEgammaKeys = getTrigEgammaKeys()


# Factory for egamaRecBuilder/TrigEgammaRecPhoton
TrigEgammaRecPhoton = AlgFactory( egammaAlgsConf.egammaRecBuilder,
        name = 'TrigEgammaRecPhoton' ,
        InputClusterContainerName = TrigEgammaKeys.precisionCaloClusterContainer, # Use as input, the clusters made by precisionCalo
        egammaRecContainer=TrigEgammaKeys.precisionEgammaRecCollection ,
        doTrackMatching = False,
        doConversions = False,
        ## Builder tools
        TrackMatchBuilderTool = TrigEMTrackMatchBuilder, # Don't want to use these for trigger....
        ConversionBuilderTool = None,  # Don't want to use these for trigger....
        doAdd = False,
        )

#Factory for photon SC builder
TrigPhotonSuperClusterBuilder = AlgFactory( egammaAlgsConf.photonSuperClusterBuilder,
        name = 'TrigPhotonSuperClusterBuilder',
        InputEgammaRecContainerName=TrigEgammaKeys.precisionEgammaRecCollection ,
        SuperPhotonRecCollectionName=TrigEgammaKeys.precisionPhotonSuperClusterRecCollection,
        SuperClusterCollectionName = TrigEgammaKeys.precisionPhotonSuperClusterCollection,
        ClusterCorrectionTool=egammaSwSuperClusterTool,
        MVACalibSvc= egammaMVASvc,
        doConversions = False,
        AddClustrsMatchingVtxTracks = False,
        ConversionBuilderTool = None,
        doAdd = False,
        LinkToConstituents = False,
        )

#Factory for photons
TrigTopoEgammaPhotons_HI = AlgFactory( egammaAlgsConf.xAODEgammaBuilder,
        name = 'TrigTopoEgammaPhotons_HI',
        InputElectronRecCollectionName = TrigEgammaKeys.precisionElectronSuperClusterRecCollection,
        InputPhotonRecCollectionName = TrigEgammaKeys.precisionPhotonSuperClusterRecCollection,
        ElectronOutputName = TrigEgammaKeys.precisionElectronContainer,
        PhotonOutputName = TrigEgammaKeys.precisionPhotonContainer,
        AmbiguityTool = EGammaAmbiguityTool,
        EMClusterTool = TrigEMClusterTool,
        EMShowerTool=TrigEMShowerBuilder_HI,
        egammaTools = FcnWrapper(TrigEgammaDecorationTools),
        PhotonTools = FcnWrapper(TrigPhotonDecorationTools),
        doAdd = False,
        doPhotons = True,
        doElectrons = False,
        )

TrigTopoEgammaPhotons = AlgFactory( egammaAlgsConf.xAODEgammaBuilder, 
        name = 'TrigTopoEgammaPhotons',
        InputElectronRecCollectionName = TrigEgammaKeys.precisionElectronSuperClusterRecCollection,
        InputPhotonRecCollectionName = TrigEgammaKeys.precisionPhotonSuperClusterRecCollection,
        ElectronOutputName = TrigEgammaKeys.precisionElectronContainer,
        PhotonOutputName = TrigEgammaKeys.precisionPhotonContainer,  
        AmbiguityTool = EGammaAmbiguityTool,
        EMClusterTool = TrigEMClusterTool,
        EMShowerTool=TrigEMShowerBuilder,
        egammaTools = FcnWrapper(TrigEgammaDecorationTools),
        PhotonTools = FcnWrapper(TrigPhotonDecorationTools),
        doAdd = False,
        doPhotons = True,
        doElectrons = False,
        )

def PrecisionPhotonTopoMonitorCfg(name = 'PrecisionPhotonTopoMonitoring'):
    
    from TrigEgammaMonitoring import TrigEgammaMonitoringConf
    from TrigEgammaMonitoring.egammaMonitorPrecisionConfig import egammaMonitorPrecisionCfg
    monTool = egammaMonitorPrecisionCfg(name)

    PrecisionPhotonTopoMonitor = AlgFactory( TrigEgammaMonitoringConf.egammaMonitorPhotonAlgorithm,
            name = name,
            doAdd = False,
            PhotonKey = TrigEgammaKeys.precisionPhotonContainer,
            MonTool = monTool
            )

    return PrecisionPhotonTopoMonitor()

def PrecisionPhotonSuperClusterMonitorCfg(name = 'PrecisionPhotonSuperClusterMonitoring'):
    
    from TrigEgammaMonitoring import TrigEgammaMonitoringConf
    from TrigEgammaMonitoring.egammaMonitorPrecisionConfig import egammaMonitorSuperClusterCfg
    monTool = egammaMonitorSuperClusterCfg(name)

    PrecisionPhotonSuperClusterMonitor = AlgFactory( TrigEgammaMonitoringConf.egammaMonitorSuperClusterAlgorithm,
            name = name,
            doAdd = False,
            InputEgammaRecContainerName = TrigEgammaKeys.precisionEgammaRecCollection,
            MonTool = monTool
            )

    return PrecisionPhotonSuperClusterMonitor()
