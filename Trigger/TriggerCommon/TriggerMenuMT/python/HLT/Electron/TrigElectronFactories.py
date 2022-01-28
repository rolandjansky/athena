# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.Logging import logging

__doc__ = "ToolFactories to configure egammaAlgs to be used at the HLT" 

"""
This file defines the factories of the algorithms to be used in an electron trigger sequence in athenaMT
These are inspired by the offline factories, alhtough modified so they reflect the configuration we need for these algorithms at the HLT. 
Offline configurations are available here:
    https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/


"""

""" Importing all read/write "DataHandles" static classes """
from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys  import getTrigEgammaKeys
TrigEgammaKeys = getTrigEgammaKeys()
from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import TrigEMClusterTool, TrigEMTrackMatchBuilder, TrigEMShowerBuilder, TrigEgammaDecorationTools

""" Importing all the tool components """
from egammaMVACalib.egammaMVACalibFactories import egammaMVASvc
from egammaTools.egammaToolsFactories import egammaSwSuperClusterTool, EGammaAmbiguityTool
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory, FcnWrapper
    
# Prepare first egammaRec:

"""Configuring egammaRecBuilder """
TrigEgammaRecElectron = AlgFactory( egammaAlgsConf.egammaRecBuilder,
                            name = 'TrigEgammaRecElectron',
                            InputClusterContainerName= "precisionCaloCluster",
                            egammaRecContainer= TrigEgammaKeys.precisionEgammaRecCollection,
                            doConversions = False,
                            doAdd= False,
                            # Builder tools
                            TrackMatchBuilderTool = TrigEMTrackMatchBuilder)

"""Configuring electronSuperClusterBuilder"""                                        
TrigElectronSuperClusterBuilder = AlgFactory( egammaAlgsConf.electronSuperClusterBuilder,
                                              name = 'TrigElectronSuperClusterBuilder',
                                              InputEgammaRecContainerName = TrigEgammaKeys.precisionEgammaRecCollection,
                                              SuperElectronRecCollectionName = TrigEgammaKeys.precisionElectronSuperClusterRecCollection,
                                              SuperClusterCollectionName = TrigEgammaKeys.precisionElectronSuperClusterCollection,
                                              ClusterCorrectionTool=egammaSwSuperClusterTool,
                                              MVACalibSvc=egammaMVASvc,
                                              EtThresholdCut=1000,
                                              TrackMatchBuilderTool = TrigEMTrackMatchBuilder,
                                              doAdd= False,
                                              LinkToConstituents = False,
                                             )



def TrigTopoEgammaElectronCfg(name='topoEgammaBuilder_TrigElectrons'):
    
    mlog = logging.getLogger(__name__)
    mlog.info('Starting configuration')
    TrigTopoEgammaElectron = AlgFactory( egammaAlgsConf.xAODEgammaBuilder, name = name,
            InputElectronRecCollectionName = TrigEgammaKeys.precisionElectronSuperClusterRecCollection,
            InputPhotonRecCollectionName = TrigEgammaKeys.precisionPhotonSuperClusterRecCollection,
            ElectronOutputName = TrigEgammaKeys.precisionElectronContainer,
            PhotonOutputName = TrigEgammaKeys.precisionPhotonContainer,  
            AmbiguityTool = EGammaAmbiguityTool,
            EMClusterTool = TrigEMClusterTool,
            EMShowerTool=TrigEMShowerBuilder,
            egammaTools = FcnWrapper(TrigEgammaDecorationTools),
            doAdd = False,
            doPhotons = False,
            doElectrons = True,
            )
    return TrigTopoEgammaElectron()

def PrecisionElectronTopoMonitorCfg(name = 'PrecisionElectronTopoMonitoring'):
    
    from TrigEgammaMonitoring import TrigEgammaMonitoringConf
    from TrigEgammaMonitoring.egammaMonitorPrecisionConfig import egammaMonitorPrecisionCfg
    monTool = egammaMonitorPrecisionCfg(name)

    PrecisionElectronTopoMonitor = AlgFactory( TrigEgammaMonitoringConf.egammaMonitorElectronAlgorithm,
            name = name,
            doAdd = False,
            ElectronKey = TrigEgammaKeys.precisionElectronContainer,
            MonTool = monTool
            )

    return PrecisionElectronTopoMonitor()

def PrecisionElectronSuperClusterMonitorCfg(name = 'PrecisionElectronSuperClusterMonitoring'):
    
    from TrigEgammaMonitoring import TrigEgammaMonitoringConf
    from TrigEgammaMonitoring.egammaMonitorPrecisionConfig import egammaMonitorSuperClusterCfg
    monTool = egammaMonitorSuperClusterCfg(name)

    PrecisionElectronSuperClusterMonitor = AlgFactory( TrigEgammaMonitoringConf.egammaMonitorSuperClusterAlgorithm,
            name = name,
            doAdd = False,
            InputEgammaRecContainerName = TrigEgammaKeys.precisionEgammaRecCollection,
            MonTool = monTool
            )

    return PrecisionElectronSuperClusterMonitor()
