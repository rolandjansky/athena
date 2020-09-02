# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def createLArRoI_Map( flags ):
    acc = ComponentAccumulator()
    LArRoI_Map=CompFactory.LArRoI_Map

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    from LArCabling.LArCablingConfig import LArFebRodMappingCfg, LArCalibIdMappingCfg

    LArCablingLegacyService=CompFactory.LArCablingLegacyService
    cablingTool = LArCablingLegacyService() # this is realy a tool
    # needed by above
    acc.merge( LArFebRodMappingCfg( flags ))
    acc.merge( LArCalibIdMappingCfg( flags ))

    CaloTriggerTowerService=CompFactory.CaloTriggerTowerService
    triggerTowerTool = CaloTriggerTowerService()                                              
    acc.merge(addFolders(flags, ['/LAR/Identifier/LArTTCellMapAtlas'], 'LAR'))
    acc.merge(addFolders(flags, ['/LAR/Identifier/OnOffIdMap'], 'LAR'))
                                       
    acc.merge(addFolders(flags, ['/CALO/Identifier/CaloTTOnOffIdMapAtlas', 
                                 '/CALO/Identifier/CaloTTOnAttrIdMapAtlas',
                                 '/CALO/Identifier/CaloTTPpmRxIdMapAtlas'], 'CALO'))
    
    LArRoI_Map = LArRoI_Map()
    LArRoI_Map.CablingSvc = cablingTool 
    LArRoI_Map.TriggerTowerSvc = triggerTowerTool
    acc.addPublicTool( LArRoI_Map ) # should become private tool
    
    return acc

def trigCaloDataAccessSvcCfg( flags ):    

    acc = ComponentAccumulator()
    svc = CompFactory.TrigCaloDataAccessSvc()

    # since the svc depends on calo geometry configure it here
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge( LArGMCfg( flags ) )

    from TileGeoModel.TileGMConfig import TileGMCfg    
    acc.merge( TileGMCfg( flags ) )

    from RegionSelector.RegSelConfig import regSelCfg
    acc.merge( regSelCfg( flags ) )
    
    acc.merge( createLArRoI_Map( flags ) )

    # Needed by bad channel maskers, refrerenced from LArCellCont.
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge(addFolders(flags, ['/LAR/BadChannels/BadChannels'], 'LAR'))
    acc.merge(addFolders(flags, ['/LAR/BadChannels/MissingFEBs'], 'LAR'))

    from TileConditions.TileEMScaleConfig import TileEMScaleCondAlgCfg
    acc.merge( TileEMScaleCondAlgCfg(flags) )

    from TileConditions.TileBadChannelsConfig import TileBadChannelsCondAlgCfg
    acc.merge( TileBadChannelsCondAlgCfg(flags) )

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
    import math
    mon = GenericMonitoringTool("TrigCaloDataAccessSvcMon")
    mon.defineHistogram("TIME_locking_LAr_RoI",
                        path="EXPERT",
                        title="Time spent in unlocking the LAr collection",
                        xbins=100, xmin=0, xmax=100 ),
    mon.defineHistogram("roiROBs_LAr",
                        path="EXPERT",
                        title="Number of ROBs unpacked in RoI requests",
                        xbins=20, xmin=0, xmax=20 ),
    mon.defineHistogram("TIME_locking_LAr_FullDet",
                        path="EXPERT",
                        title="Time spent in unlocking the LAr collection",
                        xbins=100, xmin=0, xmax=100 ),
    mon.defineHistogram("roiEta_LAr,roiPhi_LAr",
                        type="TH2F",
                        path="EXPERT",
                        title="Geometric usage",
                        xbins=50, xmin=-5, xmax=5,
                        ybins=64, ymin=-math.pi, ymax=math.pi )    
    svc.MonTool = mon
    acc.addService( svc )
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Input.isMC=False
    ConfigFlags.lock()
    acc = ComponentAccumulator()
    
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge( ByteStreamReadCfg( ConfigFlags ) )

    acc.merge( trigCaloDataAccessSvcCfg( ConfigFlags ) )
    
    TestCaloDataAccess=CompFactory.TestCaloDataAccess
    testAlg = TestCaloDataAccess()
    acc.addEventAlgo(testAlg)    
    
    # disable RegSel fro ID and muons, will change this to use flags once MR for it is integrated
    regSel = acc.getService("RegSelSvc")

    regSel.enableID    = False
    regSel.enablePixel = False
    regSel.enableSCT   = False
    regSel.enableTRT   = False
    regSel.enableMuon  = False
    regSel.enableRPC   = False
    regSel.enableMDT   = False
    regSel.enableTGC   = False
    regSel.enableCSC   = False

    acc.printConfig(True)

    print(acc.getPublicTool("LArRoI_Map"))

    print("running this configuration")
    of = open("test.pkl", "wb")
    acc.store(of)
    of.close()


    
    
