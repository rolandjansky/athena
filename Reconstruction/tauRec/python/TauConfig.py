# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType


def TauBuildAlgCfg(flags):

    result=ComponentAccumulator()

    # Tracking
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    result.merge(TrackingGeometrySvcCfg(flags))

    # Schedule total noise cond alg
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))
    # Schedule electronic noise cond alg (needed for LC weights)
    result.merge(CaloNoiseCondAlgCfg(flags,"electronicNoise"))

    # get tools from holder
    import tauRec.TauToolHolder as tauTools

    tools = []
    tools.append( result.popToolsAndMerge(tauTools.JetSeedBuilderCfg(flags)) )

    # run vertex finder only in case vertexing is available
    # if flags.InDet.doVertexFinding:
    tools.append( result.popToolsAndMerge(tauTools.TauVertexFinderCfg(flags)) )

    tools.append( result.popToolsAndMerge(tauTools.TauAxisCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.TauTrackFinderCfg(flags)) )

    tools.append( result.popToolsAndMerge(tauTools.TauClusterFinderCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.TauVertexedClusterDecoratorCfg(flags)) )

    if flags.Beam.Type is not BeamType.Cosmics:
        if flags.Tau.doRNNTrackClass:
            tools.append( result.popToolsAndMerge(tauTools.TauTrackRNNClassifierCfg(flags)) )
        tools.append( result.popToolsAndMerge(tauTools.EnergyCalibrationLCCfg(flags)) )

    tools.append( result.popToolsAndMerge(tauTools.CellVariablesCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.ElectronVetoVarsCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.TauShotFinderCfg(flags)) )

    if flags.Tau.doPi0Clus:
        tools.append( result.popToolsAndMerge(tauTools.Pi0ClusterFinderCfg(flags)) )

    # TauBuildAlg AKA TauProcessorAlg
    TauProcessorAlg=CompFactory.getComp("TauProcessorAlg")
    BuildAlg = TauProcessorAlg ( name="TauCoreBuilderAlg",
                                 Key_jetInputContainer=flags.Tau.JetCollection,
                                 Key_tauOutputContainer="tmp_TauJets",
                                 Key_tauTrackOutputContainer="TauTracks",
                                 Key_tauShotClusOutputContainer="TauShotClusters",
                                 Key_tauShotPFOOutputContainer="TauShotParticleFlowObjects",
                                 Key_tauPi0CellOutputContainer="TauCommonPi0Cells",
                                 MaxEta = flags.Tau.SeedMaxEta,
                                 MinPt = flags.Tau.SeedMinPt,
                                 MaxNTracks = flags.Tau.MaxNTracks,
                                 CellMakerTool = result.popToolsAndMerge(tauTools.TauCellFinalizerCfg(flags)) )

    BuildAlg.Tools = tools

    result.addEventAlgo(BuildAlg)  
    return result

def TauCaloAlgCfg(flags):
    result = ComponentAccumulator()

    # Schedule total noise cond alg
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))
    # Schedule electronic noise cond alg (needed for LC weights)
    result.merge(CaloNoiseCondAlgCfg(flags,"electronicNoise"))

    from CaloRec.CaloTopoClusterConfig import caloTopoCoolFolderCfg
    result.merge(caloTopoCoolFolderCfg(flags))

    from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg
    result.merge(LArBadChannelCfg(flags))

    from TileConditions.TileBadChannelsConfig import TileBadChannelsCondAlgCfg
    result.merge( TileBadChannelsCondAlgCfg(flags) )

    # get tools from holder
    import tauRec.TauToolHolder as tauTools

    CaloClusterMaker = CompFactory.getComp("CaloClusterMaker")
    CaloTopoForTausMaker = CaloClusterMaker ("TauPi0SubtractedClusterMaker")
    CaloTopoForTausMaker.ClustersOutputName = "TauPi0SubtractedClusters"
    CaloTopoForTausMaker.ClusterMakerTools = [result.popToolsAndMerge(tauTools.TauCaloTopoClusterMakerCfg(flags)),
                                              result.popToolsAndMerge(tauTools.TauCaloTopoClusterSplitterCfg(flags))]
    
    CaloTopoForTausMaker.ClusterCorrectionTools += [result.popToolsAndMerge(tauTools.TauCaloClusterBadChannelCfg(flags))]
    CaloTopoForTausMaker.ClusterCorrectionTools += [result.popToolsAndMerge(tauTools.TauCaloClusterMomentsMakerCfg(flags))]

    # calo flag to migrate - currently false
    from CaloRec.CaloTopoClusterFlags import jobproperties
    if jobproperties.CaloTopoClusterFlags.doCellWeightCalib():
        CaloTopoForTausMaker.ClusterCorrectionTools += [result.popToolsAndMerge(tauTools.TauCaloClusterCellWeightCalibCfg(flags))]

    if flags.Calo.TopoCluster.doTopoClusterLocalCalib:
        CaloTopoForTausMaker.ClusterCorrectionTools += [result.popToolsAndMerge(tauTools.TauCaloClusterLocalCalibCfg(flags)),
                                                        result.popToolsAndMerge(tauTools.TauCaloOOCPi0CalibCfg(flags)),
                                                        result.popToolsAndMerge(tauTools.TauCaloOOCCalibCfg(flags)),
                                                        result.popToolsAndMerge(tauTools.TauCaloDMCalibCfg(flags))]

    result.addEventAlgo(CaloTopoForTausMaker)
    return result

def TauRunnerAlgCfg(flags):

    result=ComponentAccumulator()

    # get tools from holder
    import tauRec.TauToolHolder as tauTools
    
    tools = []

    ### TauRecPi0EflowProcessor ###
    tools.append( result.popToolsAndMerge(tauTools.Pi0ClusterCreatorCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.Pi0ClusterScalerCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.Pi0ScoreCalculatorCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.Pi0SelectorCfg(flags)) )

    ### TauRecVariablesProcessor ###    
    # if flags.InDet.doVertexFinding:
    tools.append(result.popToolsAndMerge(tauTools.TauVertexVariablesCfg(flags)) )

    tools.append( result.popToolsAndMerge(tauTools.TauCommonCalcVarsCfg(flags)) )
    tools.append( result.popToolsAndMerge(tauTools.TauSubstructureCfg(flags)) )

    # PanTau:
    if flags.Tau.doPanTau:
        import PanTauAlgs.JobOptions_Main_PanTau_New as pantau
        tools.append( result.popToolsAndMerge(pantau.PanTauCfg(flags)) )
        # tools.append( result.popToolsAndMerge(tauTools.PanTauCfg(flags)) )

    # these tools need pantau info
    if flags.Beam.Type is not BeamType.Cosmics:
        tools.append( result.popToolsAndMerge(tauTools.MvaTESVariableDecoratorCfg(flags)) )
        tools.append( result.popToolsAndMerge(tauTools.MvaTESEvaluatorCfg(flags)) )

    if flags.Tau.doTauDiscriminant:
        tools.append( result.popToolsAndMerge(tauTools.TauIDVarCalculatorCfg(flags)) )
        tools.append( result.popToolsAndMerge(tauTools.TauJetRNNEvaluatorCfg(flags)) )
        tools.append( result.popToolsAndMerge(tauTools.TauWPDecoratorJetRNNCfg(flags)) )
        tools.append( result.popToolsAndMerge(tauTools.TauEleRNNEvaluatorCfg(flags)) )
        tools.append( result.popToolsAndMerge(tauTools.TauWPDecoratorEleRNNCfg(flags)) )
        tools.append( result.popToolsAndMerge(tauTools.TauDecayModeNNClassifierCfg(flags)) )

    tools.append( result.popToolsAndMerge(tauTools.TauAODSelectorCfg(flags)) )

    TauRunnerAlg=CompFactory.getComp("TauRunnerAlg")
    RunnerAlg = TauRunnerAlg ( name="TauRecRunnerAlg",
                               Key_tauInputContainer="tmp_TauJets",
                               Key_tauOutputContainer=flags.Tau.outputKey,
                               Key_neutralPFOOutputContainer="TauPi0Clusters",
                               Key_hadronicPFOOutputContainer="TauHadronicParticleFlowObjects",
                               Key_vertexOutputContainer = "TauSecondaryVertices",
                               Key_chargedPFOOutputContainer = "TauChargedParticleFlowObjects"
    )

    RunnerAlg.Tools = tools

    result.addEventAlgo(RunnerAlg)
    return result

if __name__=="__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    
    # from AthenaCommon.Logging import log
    # from AthenaCommon.Constants import DEBUG
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # from AthenaConfiguration.TestDefaults import defaultTestFiles

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    
    # ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]
    ConfigFlags.Output.ESDFileName="esdOut.pool.root"

    nThreads=1
    ConfigFlags.Concurrency.NumThreads = nThreads
    if nThreads>0:
        ConfigFlags.Scheduler.ShowDataDeps = True
        ConfigFlags.Scheduler.ShowDataFlow = True
        ConfigFlags.Scheduler.ShowControlFlow = True
        ConfigFlags.Concurrency.NumConcurrentEvents = nThreads

    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )
    AlgScheduler.setDataLoaderAlg('SGInputLoader')

    # Update once new jet flags are available
    from JetRec.JetRecFlags import jetFlags
    if not jetFlags.useTracks():
        ConfigFlags.Tau.doTJVA = False  # switch off TJVA

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    StoreGateSvc=CompFactory.StoreGateSvc
    cfg.addService(StoreGateSvc("DetectorStore"))

    # this delcares to the scheduler that EventInfo object comes from the input
    loadFromSG = [('xAOD::EventInfo', 'StoreGateSvc+EventInfo'),
                  ( 'AthenaAttributeList' , 'StoreGateSvc+Input' ),
                  ( 'CaloCellContainer' , 'StoreGateSvc+AllCalo' )]
    cfg.addEventAlgo(CompFactory.SGInputLoader(Load=loadFromSG), sequenceName="AthAlgSeq")

    # print "Dump flags:"
    # ConfigFlags.Tau.dump()

    tauBuildAlg = TauBuildAlgCfg(ConfigFlags)
    cfg.merge(tauBuildAlg)

    caloAlg = TauCaloAlgCfg(ConfigFlags)
    cfg.merge(caloAlg)

    tauRunnerAlg = TauRunnerAlgCfg(ConfigFlags)
    cfg.merge(tauRunnerAlg)

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    #_output     = { _outputType:_outputKey , _outputAuxType:_outputAuxKey,
    #                'xAOD::TauTrackContainer' : 'TauTracks',
    #                'xAOD::CaloClusterContainer' : 'TauShotClusters',
    #                'xAOD::PFOContainer' : 'TauShotParticleFlowObjects',
    #                'CaloCellContainer' : 'TauCommonPi0Cells',
    #}

    cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD", ItemList=["CaloClusterContainer#CaloCalTopoClusters*","xAOD::CaloClusterAuxContainer#*CaloCalTopoClusters*Aux."]))
    cfg.getEventAlgo("OutputStreamxAOD").ForceRead=True

    cfg.run(10)
