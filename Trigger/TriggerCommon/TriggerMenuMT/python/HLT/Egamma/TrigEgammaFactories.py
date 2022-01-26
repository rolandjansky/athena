# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to configure common TrigEgamma Tools" 

"""
This file defines the Tool factories of the algorithms to be used in  electron and photon triggers.
These are inspired by the offline factories, alhtough modified so they reflect the configuration we need for these algorithms at the HLT. 
Offline configurations are available here:
    https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/


"""
# athena imports
from AthenaCommon.BeamFlags import jobproperties

# flags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys

# Calo tools imports
from CaloTools.CaloToolsConf import CaloAffectedTool
from egammaCaloTools.egammaCaloToolsFactories import egammaShowerShape, egammaIso
from CaloIdentifier import SUBCALO 


# Egamma imports
from egammaRec.Factories import ToolFactory, AlgFactory, ServiceFactory
from egammaMVACalib.egammaMVACalibFactories import egammaMVASvc
from egammaTools.egammaToolsFactories import (
    egammaToolsConf, EMFourMomBuilder, PhotonPIDBuilder, egammaSwSuperClusterTool)
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools

from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__TrackParticlesInConeTool
from AthenaCommon import CfgMgr
from egammaAlgs import egammaAlgsConf

# calib svc
from egammaMVACalib import egammaMVACalibConf
from xAODEgamma.xAODEgammaParameters import xAOD

TrigEgammaKeys = getTrigEgammaKeys()
TrigEgammaKeys_LRT = getTrigEgammaKeys('_LRT') 
TrigEgammaKeys_HI = getTrigEgammaKeys(ion=True) 





def TrigEgammaMVASvcCfg( ConfigFilePath ):

    trigElectronMVATool = ToolFactory(
        egammaMVACalibConf.egammaMVACalibTool,
        name="TrigElectronMVATool",
        ParticleType=xAOD.EgammaParameters.electron,
        folder=ConfigFilePath )
    trigUnconvPhotonMVATool = ToolFactory(
        egammaMVACalibConf.egammaMVACalibTool,
        name="TrigUnconvPhotonMVATool",
        ParticleType=xAOD.EgammaParameters.unconvertedPhoton,
        folder=ConfigFilePath )
    trigConvertedPhotonMVATool = ToolFactory(
        egammaMVACalibConf.egammaMVACalibTool,
        name="TrigConvertePhotonMVATool",
        ParticleType=xAOD.EgammaParameters.convertedPhoton,
        folder=ConfigFilePath)
    trigEgammaMVASvc = ServiceFactory(
        egammaMVACalibConf.egammaMVASvc,
        name = "TrigEgammaMVASvc",
        ElectronTool=trigElectronMVATool,
        ConvertedPhotonTool=trigConvertedPhotonMVATool,
        UnconvertedPhotonTool=trigUnconvPhotonMVATool)
    return trigEgammaMVASvc

""" Configuring trigger precision MVA Svc """
TrigEgammaMVASvc = TrigEgammaMVASvcCfg( ConfigFlags.Trigger.egamma.calibMVAVersion )


"""Configuring egammaRecBuilder """
TrigEgammaRec   = AlgFactory( egammaAlgsConf.egammaRecBuilder,
                            name = 'TrigEgammaRec',
                            InputClusterContainerName = TrigEgammaKeys.precisionCaloTopoCollection,
                            egammaRecContainer        = TrigEgammaKeys.precisionCaloEgammaRecCollection,
                            doTrackMatching           = False,
                            doConversions             = False,
                            doAdd                     = False,
                            # Builder tools
                            TrackMatchBuilderTool     = None, # Don't want to use these for trigger....
                            ConversionBuilderTool     = None,  # Don't want to use these for trigger....
                            )

#Factory for egamma SC builder
TrigEgammaSuperClusterBuilder = AlgFactory( egammaAlgsConf.egammaSuperClusterBuilder,
        name = 'TrigEgammaSuperClusterBuilder',
        InputEgammaRecContainerName = TrigEgammaKeys.precisionCaloEgammaRecCollection,
        SuperClusterCollectionName  = TrigEgammaKeys.precisionCaloClusterContainer,
        ClusterCorrectionTool       = egammaSwSuperClusterTool,   
        MVACalibSvc                 = TrigEgammaMVASvc,
        CalibrationType             = 'electron',
        EtThresholdCut              = 1000,
        doAdd                       = False,
        LinkToConstituents          = False,
        )


"""Configuring the TrackParticlesInConeTool """
TrigTrackParticlesInConeTool =  ToolFactory(xAOD__TrackParticlesInConeTool, name = 'TrigTrackParticlesInConeTool')

tpict = CfgMgr.xAOD__TrackParticlesInConeTool('TrigTrackParticlesInConeTool')
tpict.TrackParticleLocation = TrigEgammaKeys.precisionTrackingContainer

"""Configuring TrackIsolationTool Tool """
TrigTrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrigTrackIsolationTool')
tit = CfgMgr.xAOD__TrackIsolationTool('TrigTrackIsolationTool')
tit.TrackSelectionTool.maxZ0SinTheta = 3
tit.TrackSelectionTool.minPt         = 1000
tit.TrackSelectionTool.CutLevel      = "Loose"
tit.TrackParticleLocation = TrigEgammaKeys.precisionTrackingContainer
tit.VertexLocation = ''
tit.TracksInConeTool      = tpict


"""Configuring the TrackParticlesInConeTool for LRT"""
TrigTrackParticlesInConeTool_LRT =  ToolFactory(xAOD__TrackParticlesInConeTool, name = 'TrigTrackParticlesInConeTool_LRT')

tpict_lrt = CfgMgr.xAOD__TrackParticlesInConeTool('TrigTrackParticlesInConeTool_LRT')
tpict_lrt.TrackParticleLocation = TrigEgammaKeys_LRT.precisionTrackingContainer

"""Configuring TrackIsolationTool Tool """
TrigTrackIsolationTool_LRT = ToolFactory(xAOD__TrackIsolationTool, name = 'TrigTrackIsolationTool_LRT')
tit_lrt = CfgMgr.xAOD__TrackIsolationTool('TrigTrackIsolationTool_LRT')
tit_lrt.TrackSelectionTool.maxZ0SinTheta = 3
tit_lrt.TrackSelectionTool.minPt         = 1000
tit_lrt.TrackSelectionTool.CutLevel      = "Loose"
tit_lrt.TrackParticleLocation = TrigEgammaKeys_LRT.precisionTrackingContainer
tit_lrt.VertexLocation = ''
tit_lrt.TracksInConeTool	  = tpict_lrt

    
"""Configuring EMTrackMatchBuilder Tool """
TrigEMTrackMatchBuilder = ToolFactory( egammaToolsConf.EMTrackMatchBuilder,
                      TrackParticlesName = TrigEgammaKeys.precisionTrackingContainer,
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

TrigEMShowerBuilder_HI = ToolFactory( egammaToolsConf.EMShowerBuilder,
         CellsName = 'CorrectedRoICaloCells',
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
        OutputClusterContainerName = TrigEgammaKeys.precisionEMClusterContainer, 
        MVACalibSvc = egammaMVASvc                             
        )

from xAODPrimitives.xAODIso import xAODIso as isoPar
from IsolationAlgs.IsolationAlgsConf import IsolationBuilder

def TrigElectronIsoBuilderCfg(name='TrigElectronIsolationBuilder'):
    TrigElectronIsolationBuilder = AlgFactory(IsolationBuilder,
                                    name                  = name,
                                    doAdd                 = False,
                                    ElectronCollectionContainerName = TrigEgammaKeys.precisionElectronContainer,
                                    CaloCellIsolationTool = None,
                                    CaloTopoIsolationTool = None,
                                    PFlowIsolationTool    = None,
                                    TrackIsolationTool    = TrigTrackIsolationTool,
                                    ElIsoTypes            = [[isoPar.ptcone20]],
                                    ElCorTypes            = [[isoPar.coreTrackPtr]],
                                    ElCorTypesExtra       = [[]],
                                    IsTrigger = True,
                                    )
    return TrigElectronIsolationBuilder()

def TrigElectronIsoBuilderCfg_LRT(name='TrigElectronIsolationBuilder_LRT'):
    TrigElectronIsolationBuilder = AlgFactory(IsolationBuilder,
                                    name                  = name,
                                    doAdd                 = False,
                                    ElectronCollectionContainerName = TrigEgammaKeys_LRT.precisionElectronContainer,
                                    CaloCellIsolationTool = None,
                                    CaloTopoIsolationTool = None,
                                    PFlowIsolationTool    = None,
                                    TrackIsolationTool    = TrigTrackIsolationTool_LRT,
                                    ElIsoTypes            = [[isoPar.ptcone20]],
                                    ElCorTypes            = [[isoPar.coreTrackPtr]],
                                    ElCorTypesExtra	      = [[]],
                                     )
    return TrigElectronIsolationBuilder()

from CaloIdentifier import SUBCALO 
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
from CaloClusterCorrection import CaloClusterCorrectionConf as Cccc
cfrc = ToolFactory(
        Cccc.CaloFillRectangularCluster,
        name="trigegamma_CaloFillRectangularCluster",
        eta_size=5,
        phi_size=7,
        cells_name='CaloCells')

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
TrigCaloClustersInConeTool = ToolFactory(xAOD__CaloClustersInConeTool, name = 'TrigCaloClustersInConeTool',
                                     CaloClusterLocation = TrigEgammaKeys.precisionTopoClusterContainer)

TrigCaloClustersInConeToolHI = ToolFactory(xAOD__CaloClustersInConeTool, name = 'TrigCaloClustersInConeToolHI',
                                     CaloClusterLocation = TrigEgammaKeys_HI.precisionTopoClusterContainer)

# this is not used below...
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ICT
IsoCorrectionTool = ToolFactory(ICT,
                                name = "TrigLeakageCorrTool")

TrigCaloIsolationToolHI = ToolFactory(xAOD__CaloIsolationTool,name = "TrigCaloIsolationToolHI",
                                postInit                        = [],
                                CaloFillRectangularClusterTool  = cfrc,
                                ClustersInConeTool              = TrigCaloClustersInConeToolHI,
                                FlowElementsInConeTool          = None,
                                ParticleCaloExtensionTool       = None,
                                IsoLeakCorrectionTool           = IsoCorrectionTool,
                                ParticleCaloCellAssociationTool = None,
                                saveOnlyRequestedCorrections    = True,
                                EMCaloNums                      = [SUBCALO.LAREM],
                                HadCaloNums                     = [SUBCALO.LARHEC, SUBCALO.TILE],
                                )
from AthenaCommon import CfgMgr
""" Configure the HLT CaloIsoTool """
H_ClIT_HI = CfgMgr.xAOD__CaloIsolationTool('TrigCaloIsolationToolHI')                                
H_ClIT_HI.doEnergyDensityCorrection=True
H_ClIT_HI.InitializeReadHandles=False
H_ClIT_HI.UseEMScale=True

TrigCaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "TrigCaloIsolationTool",
                                postInit                        = [],
                                CaloFillRectangularClusterTool  = cfrc,
                                ClustersInConeTool              = TrigCaloClustersInConeTool,
                                FlowElementsInConeTool          = None,
                                ParticleCaloExtensionTool       = None,
                                IsoLeakCorrectionTool           = IsoCorrectionTool,
                                ParticleCaloCellAssociationTool = None,
                                saveOnlyRequestedCorrections    = True,
                                EMCaloNums                      = [SUBCALO.LAREM],
                                HadCaloNums                     = [SUBCALO.LARHEC, SUBCALO.TILE],
                                )
from AthenaCommon import CfgMgr
""" Configure the HLT CaloIsoTool """
H_ClIT = CfgMgr.xAOD__CaloIsolationTool('TrigCaloIsolationTool')                                
H_ClIT.doEnergyDensityCorrection=True
H_ClIT.InitializeReadHandles=False
H_ClIT.UseEMScale=True


def TrigPhotonIsoBuilderCfg(name='TrigPhotonIsolationBuilder'):
    TrigPhotonIsolationBuilder = AlgFactory(IsolationBuilder,
                                    name                  = name,
                                    doAdd                 = False,
                                    PhotonCollectionContainerName = TrigEgammaKeys.precisionPhotonContainer,
                                    CaloCellIsolationTool = None,
                                    CaloTopoIsolationTool = TrigCaloIsolationTool,
                                    PFlowIsolationTool    = None,
                                    TrackIsolationTool    = None, 
                                    PhIsoTypes            = [[isoPar.topoetcone20, isoPar.topoetcone40]],
                                    PhCorTypes            = [[isoPar.core57cells]],
                                    PhCorTypesExtra       = [[]],
                                    )
    return TrigPhotonIsolationBuilder()

def TrigPhotonIsoBuilderHICfg(name='TrigPhotonIsolationBuilderHI'):
    TrigPhotonIsolationBuilder = AlgFactory(IsolationBuilder,
                                    name                  = name,
                                    doAdd                 = False,
                                    PhotonCollectionContainerName = TrigEgammaKeys.precisionPhotonContainer,
                                    CaloCellIsolationTool = None,
                                    CaloTopoIsolationTool = TrigCaloIsolationToolHI,
                                    PFlowIsolationTool    = None,
                                    TrackIsolationTool    = None, 
                                    PhIsoTypes            = [[isoPar.topoetcone20, isoPar.topoetcone40]],
                                    PhCorTypes            = [[isoPar.core57cells]],
                                    PhCorTypesExtra       = [[]],
                                    )
    return TrigPhotonIsolationBuilder()




def TrigEgammaDecorationTools():
    #Return a list with the tools that decorate both electrons and photons. 
    return [ EMFourMomBuilder() ] 

def TrigPhotonDecorationTools():
    #Return a list with the tools that decorate only photons
    return [ PhotonPIDBuilder() ]

def egammaFSCaloRecoSequence():
    from AthenaCommon.CFElements import parOR

    from TrigT2CaloCommon.CaloDef import setMinimalCaloSetup
    setMinimalCaloSetup()

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker

    cellMaker = HLTCaloCellMaker('HLTCaloCellMakerEGFS')
    cellMaker.RoIs = mapThresholdToL1RoICollection('FSNOSEED')
    cellMaker.TrigDataAccessMT = svcMgr.TrigCaloDataAccessSvc
    cellMaker.CellsName = 'CaloCellsEGFS'

    from TrigT2CaloCommon.CaloDef import _algoHLTHIEventShape
    eventShapeMaker = _algoHLTHIEventShape(
        name='HLTEventShapeMakerEG',
        inputEDM=cellMaker.CellsName,
        outputEDM=TrigEgammaKeys.egEventShape
    )

    return parOR("egammaFSRecoSequence", [cellMaker, eventShapeMaker])
