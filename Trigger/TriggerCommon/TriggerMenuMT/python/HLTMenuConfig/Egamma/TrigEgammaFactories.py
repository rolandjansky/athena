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
from egammaRec.Factories import ToolFactory, AlgFactory

from egammaTools.egammaToolsFactories import egammaToolsConf, egammaSwSuperClusterTool, egammaMVASvc,  EMFourMomBuilder, PhotonPIDBuilder, ElectronPIDBuilder

from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools

# Load TrigEgammaKeys where we store the container names and other TrigEgamma configuration values
from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys

from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__TrackParticlesInConeTool

from AthenaCommon import CfgMgr
"""Configuring the TrackParticlesInConeTool """
TrigTrackParticlesInConeTool =  ToolFactory(xAOD__TrackParticlesInConeTool, name = 'TrigTrackParticlesInConeTool')

tpict = CfgMgr.xAOD__TrackParticlesInConeTool('TrigTrackParticlesInConeTool')
tpict.TrackParticleLocation = TrigEgammaKeys.TrigElectronTracksCollectionName

"""Configuring TrackIsolationTool Tool """
TrigTrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrigTrackIsolationTool')
tit = CfgMgr.xAOD__TrackIsolationTool('TrigTrackIsolationTool')
tit.TrackSelectionTool.maxZ0SinTheta = 3
tit.TrackSelectionTool.minPt         = 1000
tit.TrackSelectionTool.CutLevel      = "Loose"
tit.TrackParticleLocation = TrigEgammaKeys.TrigElectronTracksCollectionName
tit.VertexLocation = ''
tit.TracksInConeTool      = tpict



"""Configuring EMTrackMatchBuilder Tool """
TrigEMTrackMatchBuilder = ToolFactory( egammaToolsConf.EMTrackMatchBuilder,
                      TrackParticlesName = TrigEgammaKeys.TrigElectronTracksCollectionName,
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
from xAODPrimitives.xAODIso import xAODIso as isoPar
from IsolationAlgs.IsolationAlgsConf import IsolationBuilder

def TrigElectronIsoBuilderCfg(name='TrigElectronIsolationBuilder'):
    TrigElectronIsolationBuilder = AlgFactory(IsolationBuilder,
                                    name                  = name,
                                    doAdd = False,
                                    ElectronCollectionContainerName = 'HLT_egamma_Electrons',
                                    CaloCellIsolationTool = None,
                                    CaloTopoIsolationTool = None,
                                    PFlowIsolationTool    = None,
                                    TrackIsolationTool    = TrigTrackIsolationTool,
                                    FeIsoTypes            = [[]],
                                    FeCorTypes            = [[]],
                                    FeCorTypesExtra       = [[]],
                                    ElIsoTypes            = [[isoPar.ptcone20]],
                                    ElCorTypes            = [[]],
                                    ElCorTypesExtra       = [[]],
                                     )
    return TrigElectronIsolationBuilder()


def TrigEgammaDecorationTools():
    #Return a list with the tools that decorate both electrons and photons. 
    return [ EMFourMomBuilder() ] 

def TrigPhotonDecorationTools():
    #Return a list with the tools that decorate only photons
    return [ PhotonPIDBuilder() ]

def TrigElectronDecorationTools():
    #Return a list with the tools that decorate only photons
    return [ ElectronPIDBuilder()]

