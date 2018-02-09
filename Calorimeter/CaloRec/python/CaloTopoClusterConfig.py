# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ConfigFlags import ConfigFlagContainer
from AthenaCommon.SystemOfUnits import MeV

def CaloTopoClusterCfg(inputFlags):
    result=ComponentAccumulator()
    
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    
    #from CaloUtils.CaloUtilsConf import CaloLCClassificationTool, CaloLCWeightTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool

    from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
    from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterCellWeightCalib
    from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloClusterSnapshot #, CaloClusterLockVars, CaloClusterPrinter

    
    result.executeModule(LArGMCfg,inputFlags)
    result.executeModule(TileGMCfg,inputFlags)    
    result.executeModule(CaloNoiseToolCfg,inputFlags)

    theCaloNoiseTool=result.getPublicTool("CaloNoiseTool")

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
    TopoMaker.CaloNoiseTool=theCaloNoiseTool
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
    TopoMaker.TwoGaussianNoise = inputFlags.get("CaloRec.CaloTopoClusterConfigFlags.doTwoGaussianNoise")
        
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
    TopoSplitter.WeightingOfNegClusters = inputFlags.get("CaloRec.CaloTopoClusterConfigFlags.doTreatEnergyCutAsAbsolute")
    #
    # the following options are not set, since these are the default
    # values
    #
    # NeighborOption                = "super3D",
    # NumberOfCellsCut              = 4,
    # EnergyCut                     = 500*MeV,
        

    CaloTopoCluster=CaloClusterMaker("CaloTopoCluster")
    CaloTopoCluster.ClustersOutputName="CaloCalTopoClusters"   

    CaloTopoCluster.ClusterMakerTools = [TopoMaker, TopoSplitter]
            
    result.addEventAlgo(CaloTopoCluster)
    return result



if __name__=="__main__":
    cfg=ComponentAccumulator()
    from AthenaConfiguration.CfgLogMsg import cfgLogMsg
    cfgLogMsg.setLevel("debug")
    cfgFlags=ConfigFlagContainer()
    cfgFlags.set("AthenaConfiguration.GlobalFlags.isMC",False)
    cfgFlags.set("AthenaConfiguration.GlobalFlags.InputFiles",["myESD.pool.root"])

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.executeModule(PoolReadCfg,cfgFlags)

    cfg.executeModule(CaloTopoClusterCfg,cfgFlags)
    cfg.getEventAlgo("CaloTopoCluster").ClustersOutputName="CaloCalTopoClustersNew" 

    f=open("CaloTopoCluster.pkl","w")
    cfg.store(f)
    f.close()
    
