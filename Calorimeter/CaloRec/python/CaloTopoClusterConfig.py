# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from AthenaCommon.SystemOfUnits import MeV

def CaloTopoClusterCfg(inputFlags=None):
    result=ComponentAccumulator()
    
    
    #from CaloUtils.CaloUtilsConf import CaloLCClassificationTool, CaloLCWeightTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool

    #from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
    #from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterCellWeightCalib


    from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloClusterSnapshot #, CaloClusterLockVars, CaloClusterPrinter

    

    #from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    #theCaloNoiseTool = CaloNoiseToolDefault()

    # maker tools
    TopoMaker = CaloTopoClusterMaker("TopoMaker")
        
    TopoMaker.CellsName = "AllCalo"
    TopoMaker.CalorimeterNames=["LAREM",
                                "LARHEC",
                                "LARFCAL",
                                "TILE"]
    # cells from the following samplings will be able to form
    # seeds. By default no sampling is excluded
    TopoMaker.SeedSamplingNames = ["PreSamplerB", "EMB1", "EMB2", "EMB3",
                                   "PreSamplerE", "EME1", "EME2", "EME3",
                                   "HEC0", "HEC1","HEC2", "HEC3",
                                   "TileBar0", "TileBar1", "TileBar2",
                                   "TileExt0", "TileExt1", "TileExt2",
                                   "TileGap1", "TileGap2", "TileGap3",
                                   "FCAL0", "FCAL1", "FCAL2"] 
    #TopoMaker.CaloNoiseTool=theCaloNoiseTool
    TopoMaker.UseCaloNoiseTool=True
    TopoMaker.UsePileUpNoise=True
    TopoMaker.NeighborOption = "super3D"
    TopoMaker.RestrictHECIWandFCalNeighbors  = False
    TopoMaker.RestrictPSNeighbors  = True
    TopoMaker.CellThresholdOnEorAbsEinSigma     =    0.0
    TopoMaker.NeighborThresholdOnEorAbsEinSigma =    2.0
    TopoMaker.SeedThresholdOnEorAbsEinSigma     =    4.0
    
    # note E or AbsE 
    #
    # the following property must be set to TRUE in order to make double
    # sided cuts on the seed and the cluster level 
    #
    TopoMaker.SeedCutsInAbsE                 = True
    TopoMaker.ClusterEtorAbsEtCut            = 0.0*MeV
    # use 2-gaussian or single gaussian noise for TileCal
    TopoMaker.TwoGaussianNoise = inputFlags.CaloTopoClusterFlags.doTwoGaussianNoise()
        
    TopoSplitter = CaloTopoClusterSplitter("TopoSplitter")
    # cells from the following samplings will be able to form local
    # maxima. The excluded samplings are PreSamplerB, EMB1,
    # PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
    # two rear FCal samplings.
    #
    TopoSplitter.SamplingNames = ["EMB2", "EMB3",
                                  "EME2", "EME3",
                                  "FCAL0"]
    # cells from the following samplings will also be able to form
    # local maxima but only if they are not overlapping in eta and phi
    # with local maxima in previous samplings from the primary list.
    #
    TopoSplitter.SecondarySamplingNames = ["EMB1","EME1",
                                           "TileBar0","TileBar1","TileBar2",
                                           "TileExt0","TileExt1","TileExt2",
                                           "HEC0","HEC1","HEC2","HEC3",
                                           "FCAL1","FCAL2"]
    TopoSplitter.ShareBorderCells = True
    TopoSplitter.RestrictHECIWandFCalNeighbors  = False
    TopoSplitter.WeightingOfNegClusters = inputFlags.CaloTopoClusterFlags.doTreatEnergyCutAsAbsolute()
    #
    # the following options are not set, since these are the default
    # values
    #
    # NeighborOption                = "super3D",
    # NumberOfCellsCut              = 4,
    # EnergyCut                     = 500*MeV,
        

    CaloTopoCluster=CaloClusterMaker("CaloTopoCluster")
    CaloTopoCluster.ClustersOutputName="CaloCalTopoClusters"    # cluster maker

    CaloTopoCluster.ClusterMakerTools = [TopoMaker, TopoSplitter]
            
    result.addEventAlgo(CaloTopoCluster)
    return result

