# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CaloRecoCfg(configFlags):
    
    result=ComponentAccumulator()
    if not configFlags.Input.isMC:
        #Data-case: Schedule ByteStream reading for LAr & Tile
        from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
        result.merge(LArRawDataReadingCfg(configFlags))


        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        result.merge( ByteStreamReadCfg(configFlags,typeNames=['TileDigitsContainer/TileDigitsCnt','TileRawChannelContainer/TileRawChannelCnt']))


        from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
        result.merge(LArRawChannelBuilderAlgCfg(configFlags))

        
        from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
        result.merge( TileRawChannelMakerCfg(configFlags) )

              
    #Configure cell-building
    from CaloRec.CaloCellMakerConfig import CaloCellMakerCfg
    result.merge(CaloCellMakerCfg(configFlags))
    
    #Configure topo-cluster builder
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    result.merge(CaloTopoClusterCfg(configFlags))

    return result


if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data",]
    ConfigFlags.lock()


    acc=CaloRecoCfg(ConfigFlags)


    from CaloRec.CaloRecConf import CaloCellDumper
    acc.addEventAlgo(CaloCellDumper())

    from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterDumper
    acc.addEventAlgo(ClusterDumper("TopoDumper",ContainerName="CaloCalTopoClusters",FileName="TopoCluster.txt"))

    f=open("CaloRec.pkl","w")
    acc.store(f)
    f.close()
