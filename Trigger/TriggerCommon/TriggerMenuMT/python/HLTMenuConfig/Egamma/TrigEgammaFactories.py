# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to configure common TrigEgamma Tools" 

"""
This file defines the Tool factories of the algorithms to be used in  electron and photon triggers.
These are inspired by the offline factories, alhtough modified so they reflect the configuration we need for these algorithms at the HLT. 
Offline configurations are available here:
    https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/


"""
# athena imports
from AthenaCommon.BeamFlags import jobproperties

# Calo tools imports
from CaloTools.CaloToolsConf import CaloAffectedTool
from egammaCaloTools.egammaCaloToolsFactories import egammaShowerShape, egammaIso
from CaloIdentifier import SUBCALO 

# Egamma imports
from egammaRec.Factories import ToolFactory
from egammaTools.egammaToolsFactories import egammaToolsConf, egammaSwSuperClusterTool, egammaMVASvc,  EMFourMomBuilder, PhotonPIDBuilder, ElectronPIDBuilder
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools

# Load TrigEgammaKeys where we store the container names and other TrigEgamma configuration values
from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys


"""Configuring EMTrackMatchBuilder Tool """
TrigEMTrackMatchBuilder = ToolFactory( egammaToolsConf.EMTrackMatchBuilder,
                      TrackParticlesName = "HLT_IDTrack_Electron_IDTrig",
                      ExtrapolationTool  = EMExtrapolationTools,
                      broadDeltaEta      = 0.1, #candidate match is done in 2 times this  so +- 0.2
                      broadDeltaPhi      = 0.15,  #candidate match is done in 2 times this  so +- 0.3
                      useCandidateMatch  = True,
                      useScoring         = True,
                      SecondPassRescale  = True,
                      UseRescaleMetric   = True,
                      isCosmics          = (jobproperties.Beam.beamType()=="cosmics"))

"""Configuring the builder of Egamma shower shapes"""
TrigEMShowerBuilder = ToolFactory( egammaToolsConf.EMShowerBuilder,
         CellsName = 'CaloCells',
         CaloNums  = [SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE],
         ShowerShapeTool      = egammaShowerShape,
         HadronicLeakageTool  = egammaIso,
         Print = False)


"""Configure the ObjectQuality tool"""
TrigEgammaOQFlagsBuilder = ToolFactory( egammaToolsConf.egammaOQFlagsBuilder,
        QCellCut = 4000.,
        QCellHECCut = 60000.,
        QCellSporCut = 4000.,
        LArQCut = 0.8,
        TCut = 10.,
        TCutVsE = 2.,
        RcellCut = 0.8,
        CellsName = 'CaloCells',
        affectedTool = CaloAffectedTool())

""" This is an instance of TrigEMClusterTool to be used at TrigTopoEgammaPhotons and TrigTopoEgammaElectrons """
TrigEMClusterTool = ToolFactory(egammaToolsConf.EMClusterTool,
        name = 'TrigEMClusterTool',
        OutputClusterContainerName = TrigEgammaKeys.TrigEMClusterToolOutputContainer, 
        OutputTopoSeededClusterContainerName = TrigEgammaKeys.outputTopoSeededClusterKey,
        ClusterCorrectionTool = egammaSwSuperClusterTool,
        doSuperCluster = True,
        MVACalibSvc = egammaMVASvc                             
        )

def TrigEgammaDecorationTools():
    #Return a list with the tools that decorate both electrons and photons. 
    return [ EMFourMomBuilder() ] 

def TrigPhotonDecorationTools():
    #Return a list with the tools that decorate only photons
    return [ PhotonPIDBuilder() ]

def TrigElectronDecorationTools():
    #Return a list with the tools that decorate only photons
    return [ ElectronPIDBuilder() ]

