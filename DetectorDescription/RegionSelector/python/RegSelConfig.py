#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def regSelCfg( flags ):
    """ Configures Region Selector Svc according to the detector flags """
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from RegionSelector.RegionSelectorConf import RegSelSvc
    from AthenaCommon.SystemOfUnits import mm
    acc = ComponentAccumulator()

    regSel = RegSelSvc()
    regSel.DeltaZ = 225 * mm

    if flags.Detector.GeometryLAr:
        regSel.enableCalo = True

        from LArGeoAlgsNV.LArGMConfig import LArGMCfg
        acc.merge( LArGMCfg(  flags ) )

        from LArRegionSelector.LArRegionSelectorConf import LArRegionSelectorTable
        larTable =  LArRegionSelectorTable(name="LArRegionSelectorTable")
        acc.addPublicTool( larTable )
        regSel.LArRegionSelectorTable      = larTable
        
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        acc.merge( addFolders(flags, ['/LAR/Identifier/FebRodMap'], 'LAR' ))

    if flags.Detector.GeometryTile:
        regSel.enableCalo = True

        from TileGeoModel.TileGMConfig import TileGMCfg
        acc.merge( TileGMCfg( flags ) )

        from TileRawUtils.TileRawUtilsConf import TileRegionSelectorTable
        tileTable =  TileRegionSelectorTable(name="TileRegionSelectorTable")
        acc.addPublicTool( tileTable )

    if flags.Detector.GeometryPixel:
        regSel.enableID = True
        regSel.enablePixel = True        
        from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
        pixTable = SiRegionSelectorTable(name        = "PixelRegionSelectorTable",
                                         ManagerName = "Pixel",
                                         OutputFile  = "RoITablePixel.txt",
                                         PrintHashId = True,
                                         PrintTable  = False)
        acc.addPublicTool(pixTable)

    if flags.Detector.GeometrySCT:
        regSel.enableID = True
        regSel.enableSCT = True

        from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
        sctTable = SiRegionSelectorTable(name        = "SCT_RegionSelectorTable",
                                         ManagerName = "SCT",
                                         OutputFile  = "RoITableSCT.txt",
                                         PrintHashId = True,
                                         PrintTable  = False)
        acc.addPublicTool(sctTable)

    if flags.Detector.GeometryTRT:
        regSel.enableID = True
        regSel.enableTRT = True

        from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegionSelectorTable
        trtTable = TRT_RegionSelectorTable(name = "TRT_RegionSelectorTable",
                                           ManagerName = "TRT",
                                           OutputFile  = "RoITableTRT.txt",
                                           PrintHashId = True,
                                           PrintTable  = False)
        acc.addPublicTool(trtTable)


    if flags.Detector.GeometryRPC:
        regSel.enableMuon = True
        regSel.enableRPC  = True
        from MuonRegionSelector.MuonRegionSelectorConf import RPC_RegionSelectorTable
        rpcTable = RPC_RegionSelectorTable(name = "RPC_RegionSelectorTable")
        acc.addPublicTool( rpcTable )

    if flags.Detector.GeometryMDT:
        regSel.enableMuon = True
        regSel.enableMDT  = True
        from MuonRegionSelector.MuonRegionSelectorConf import MDT_RegionSelectorTable
        mdtTable = MDT_RegionSelectorTable(name = "MDT_RegionSelectorTable")
        acc.addPublicTool( mdtTable )

    if flags.Detector.GeometryTGC:
        regSel.enableMuon = True
        regSel.enableTGC  = True
        from MuonRegionSelector.MuonRegionSelectorConf import TGC_RegionSelectorTable
        tgcTable = TGC_RegionSelectorTable(name = "TGC_RegionSelectorTable")
        acc.addPublicTool( tgcTable )

    if flags.Detector.GeometryCSC:
        regSel.enableMuon = True
        regSel.enableCSC  = True
        from MuonRegionSelector.MuonRegionSelectorConf import CSC_RegionSelectorTable
        cscTable = CSC_RegionSelectorTable(name = "CSC_RegionSelectorTable")
        acc.addPublicTool( cscTable )


    if flags.Detector.GeometryMM:
        regSel.enableMM  = True   

    acc.addService( regSel )



    return acc

if __name__ == "__main__":
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Configurable import Configurable
    from AthenaCommon.Constants import DEBUG

    Configurable.configurableRun3Behavior=True

    ConfigFlags.Detector.GeometryPixel = True     
    ConfigFlags.Detector.GeometrySCT   = True 
    ConfigFlags.Detector.GeometryTRT   = True 
    ConfigFlags.Detector.GeometryLAr   = True     
    ConfigFlags.Detector.GeometryTile  = True     
    ConfigFlags.Detector.GeometryMDT   = True 
    ConfigFlags.Detector.GeometryTGC   = True
    ConfigFlags.Detector.GeometryCSC   = True     
    ConfigFlags.Detector.GeometryRPC   = True     
    

    ConfigFlags.Input.Files = defaultTestFiles.RAW    
    ConfigFlags.Input.isMC = False
    ConfigFlags.dump()
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    cfg=MainServicesSerialCfg() 

    ## move up

    # when trying AOD
    #    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    #    cfg.merge( PoolReadCfg( ConfigFlags ) )

    
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    cfg.merge(TrigBSReadCfg( ConfigFlags ))
    
    
    acc = regSelCfg( ConfigFlags )
    cfg.merge( acc )


    from RegSelSvcTest.RegSelSvcTestConf import RegSelTestAlg
    testerAlg = RegSelTestAlg()
    testerAlg.Mt=True    
    testerAlg.OutputLevel=DEBUG
    cfg.addEventAlgo( testerAlg )
    from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
    apcs=AthenaPoolCnvSvc()
    cfg.addService(apcs)
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    cfg.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[apcs.getFullJobOptName(),]))

    cfg.getService("IOVDbSvc").OutputLevel=DEBUG
    
    cfg.store( file( "test.pkl", "w" ) )
    print "used flags"
    ConfigFlags.dump()
    cfg.run(0)
    print "All OK"
