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

""" Importing all the tool components """
from egammaTools.egammaToolsFactories import egammaToolsConf, egammaSwTool, egammaMVASvc, EGammaAmbiguityTool
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
from egammaAlgs import egammaAlgsConf
from AthenaCommon.BeamFlags import jobproperties
from egammaRec.Factories import ToolFactory, AlgFactory
# Prepare first egammaRec:
    
"""Configuring EMTrackMatchBuilder Tool """
EMTrackMatchBuilder = ToolFactory( egammaToolsConf.EMTrackMatchBuilder,
                      TrackParticlesName = "InDetTrigPTTrackParticles_electron",
                      ExtrapolationTool  = EMExtrapolationTools,
                      broadDeltaEta      = 0.1, #candidate match is done in 2 times this  so +- 0.2
                      broadDeltaPhi      = 0.15,  #candidate match is done in 2 times this  so +- 0.3
                      useCandidateMatch  = True,
                      useScoring         = True,
                      SecondPassRescale  = True,
                      UseRescaleMetric   = True,
                      isCosmics          = (jobproperties.Beam.beamType()=="cosmics"))

"""Configuring egammaRecBuilder """
TrigEgammaRecElectron = AlgFactory( egammaAlgsConf.egammaRecBuilder,
                            name = 'TrigEgammaRecElectron',
                            InputTopoClusterContainerName= "precisionCaloCluster",
                            egammaRecContainer= TrigEgammaKeys.EgammaRecKey,
                            doConversions = False,
                            doAdd= False,
                            # Builder tools
                            TrackMatchBuilderTool = EMTrackMatchBuilder)

"""Configuring electronSuperClusterBuilder"""                                        
TrigElectronSuperClusterBuilder = AlgFactory( egammaAlgsConf.electronSuperClusterBuilder,
                                              name = 'TrigElectronSuperClusterBuilder',
                                              InputEgammaRecContainerName = TrigEgammaKeys.EgammaRecKey,
                                              SuperElectronRecCollectionName = TrigEgammaKeys.SuperElectronRecCollectionName,
                                              ClusterCorrectionTool=egammaSwTool,
                                              MVACalibSvc=egammaMVASvc,
                                              EtThresholdCut=1000,
                                              TrackMatchBuilderTool = EMTrackMatchBuilder,
                                              doAdd= False
                                             )

"""This is an instance of TrigEMClusterTool to be used at TrigTopoEgammaElectrons"""
TrigEMClusterTool = ToolFactory(egammaToolsConf.EMClusterTool,
        name = 'TrigEMEClusterTool',
        OutputClusterContainerName = TrigEgammaKeys.TrigEMClusterToolOutputContainer,
        OutputTopoSeededClusterContainerName = TrigEgammaKeys.outputTopoSeededClusterKey,
        ClusterCorrectionTool = egammaSwTool,
        doSuperCluster = True,
        MVACalibSvc = egammaMVASvc
        )


TrigTopoEgammaElectron = AlgFactory( egammaAlgsConf.topoEgammaBuilder, name = 'TrigTopoEgammaElectron',
        SuperElectronRecCollectionName = TrigEgammaKeys.SuperElectronRecCollectionName,
        SuperPhotonRecCollectionName = TrigEgammaKeys.SuperPhotonRecCollectionName,
        ElectronOutputName = TrigEgammaKeys.outputElectronKey,
        PhotonOutputName = TrigEgammaKeys.outputPhotonKey,  
        AmbiguityTool = EGammaAmbiguityTool,
        EMClusterTool = TrigEMClusterTool,
        doAdd = False,
        doPhotons = False,
        doElectrons = True
        )


