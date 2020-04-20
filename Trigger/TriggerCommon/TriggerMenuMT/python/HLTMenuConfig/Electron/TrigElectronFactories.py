# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to configure egammaAlgs to be used at the HLT" 

"""
This file defines the factories of the algorithms to be used in an electron trigger sequence in athenaMT
These are inspired by the offline factories, alhtough modified so they reflect the configuration we need for these algorithms at the HLT. 
Offline configurations are available here:
    https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/


"""

""" Importing all read/write "DataHandles" static classes """
from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigEMClusterTool, TrigEMTrackMatchBuilder, TrigEMShowerBuilder, TrigEgammaDecorationTools

""" Importing all the tool components """
from egammaTools.egammaToolsFactories import egammaSwTool, egammaMVASvc, EGammaAmbiguityTool
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory, FcnWrapper
    
# Prepare first egammaRec:

"""Configuring egammaRecBuilder """
TrigEgammaRecElectron = AlgFactory( egammaAlgsConf.egammaRecBuilder,
                            name = 'TrigEgammaRecElectron',
                            InputTopoClusterContainerName= "precisionCaloCluster",
                            egammaRecContainer= TrigEgammaKeys.EgammaRecKey,
                            doConversions = False,
                            doAdd= False,
                            # Builder tools
                            TrackMatchBuilderTool = TrigEMTrackMatchBuilder)

"""Configuring electronSuperClusterBuilder"""                                        
TrigElectronSuperClusterBuilder = AlgFactory( egammaAlgsConf.electronSuperClusterBuilder,
                                              name = 'TrigElectronSuperClusterBuilder',
                                              InputEgammaRecContainerName = TrigEgammaKeys.EgammaRecKey,
                                              SuperElectronRecCollectionName = TrigEgammaKeys.SuperElectronRecCollectionName,
                                              ClusterCorrectionTool=egammaSwTool,
                                              MVACalibSvc=egammaMVASvc,
                                              EtThresholdCut=1000,
                                              TrackMatchBuilderTool = TrigEMTrackMatchBuilder,
                                              doAdd= False
                                             )


TrigTopoEgammaElectron = AlgFactory( egammaAlgsConf.topoEgammaBuilder, name = 'TrigTopoEgammaElectron',
        SuperElectronRecCollectionName = TrigEgammaKeys.SuperElectronRecCollectionName,
        SuperPhotonRecCollectionName = TrigEgammaKeys.SuperPhotonRecCollectionName,
        ElectronOutputName = TrigEgammaKeys.outputElectronKey,
        PhotonOutputName = TrigEgammaKeys.outputPhotonKey,  
        AmbiguityTool = EGammaAmbiguityTool,
        EMClusterTool = TrigEMClusterTool,
        EMShowerTool=TrigEMShowerBuilder,
        egammaTools = FcnWrapper(TrigEgammaDecorationTools),
        doAdd = False,
        doPhotons = False,
        doElectrons = True
        )


