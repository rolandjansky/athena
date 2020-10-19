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

from egammaTools.egammaToolsFactories import egammaSwSuperClusterTool, egammaMVASvc, EGammaAmbiguityTool

# Tools and funtions from TrigEgammaFactories
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigEMClusterTool, TrigEMShowerBuilder ,TrigEgammaDecorationTools, TrigPhotonDecorationTools, TrigEMTrackMatchBuilder

# Load TrigEgammaKeys where we store the container names and other TrigEgamma configuration values
from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs


# Decoration tools for egamma and photon objects:
# Copying from https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaRec/python/topoEgammaGetter.py#L28


# Factory for egamaRecBuilder/TrigEgammaRecPhoton
TrigEgammaRecPhoton = AlgFactory( egammaAlgsConf.egammaRecBuilder,
        name = 'TrigEgammaRecPhoton' ,
        InputTopoClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters, # Use as input, the clusters made by precisionCalo
        egammaRecContainer=TrigEgammaKeys.EgammaRecKey,
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
        InputEgammaRecContainerName=TrigEgammaKeys.EgammaRecKey,
        SuperPhotonRecCollectionName=TrigEgammaKeys.SuperPhotonRecCollectionName,
        ClusterCorrectionTool=egammaSwSuperClusterTool,
        MVACalibSvc= egammaMVASvc,
        doConversions = False,
        AddClustrsMatchingVtxTracks = False,
        ConversionBuilderTool = None,
        doAdd = False
        )

#Factory for photons
TrigTopoEgammaPhotons = AlgFactory( egammaAlgsConf.topoEgammaBuilder, 
        name = 'TrigTopoEgammaPhotons',
        SuperElectronRecCollectionName = TrigEgammaKeys.SuperElectronRecCollectionName,
        SuperPhotonRecCollectionName = TrigEgammaKeys.SuperPhotonRecCollectionName,
        ElectronOutputName = TrigEgammaKeys.outputElectronKey,
        PhotonOutputName = TrigEgammaKeys.outputPhotonKey,  
        AmbiguityTool = EGammaAmbiguityTool,
        EMClusterTool = TrigEMClusterTool,
        EMShowerTool=TrigEMShowerBuilder,
        egammaTools = FcnWrapper(TrigEgammaDecorationTools),
        PhotonTools = FcnWrapper(TrigPhotonDecorationTools),
        doAdd = False,
        doPhotons = True,
        doElectrons = False,
        )

def PrecisionPhotonTopoMonitorCfg(name = 'PrecisionPhotonTopoEgammaBuilder'):
    
    from TrigEgammaMonitoring import TrigEgammaMonitoringConf
    from TrigEgammaMonitoring.egammaMonitorPrecisionConfig import egammaMonitorPrecisionCfg
    monTool = egammaMonitorPrecisionCfg(name)

    PrecisionPhotonTopoMonitor = AlgFactory( TrigEgammaMonitoringConf.egammaMonitorPhotonAlgorithm,
            name = name,
            doAdd = False,
            PhotonKey = TrigEgammaKeys.outputPhotonKey,
            MonTool = monTool
            )

    return PrecisionPhotonTopoMonitor()
