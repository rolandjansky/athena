# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
def createLArRoI_Map( flags ):
    acc = ComponentAccumulator()
    from LArRawUtils.LArRawUtilsConf import LArRoI_Map

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    from LArCabling.LArCablingConfig import LArFebRodMappingCfg, LArCalibIdMappingCfg
    # LArOnOffIdMappingCfg, LArOnOffIdMappingSCCfg 

    from LArCabling.LArCablingConf import LArCablingLegacyService
    cablingTool = LArCablingLegacyService() # this is realy a tool
    # needed by above
    acc.merge( LArFebRodMappingCfg( flags )[0] )
    acc.merge( LArCalibIdMappingCfg( flags )[0] )

    from CaloTriggerTool.CaloTriggerToolConf import CaloTriggerTowerService
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
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
    svc = TrigCaloDataAccessSvc()

    # since the svc depends on calo geometry configure it here
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge( LArGMCfg( flags ) )

    from TileGeoModel.TileGMConfig import TileGMCfg    
    acc.merge( TileGMCfg( flags ) )
    acc.getService('GeoModelSvc').DetectorTools['TileDetectorTool'].GeometryConfig = 'RECO'

    from RegionSelector.RegSelConfig import RegSelConfig
    acc.mergeAll( RegSelConfig( flags ) )
    
    acc.merge( createLArRoI_Map( flags ) )

    # Needed by LArBadChanLegacyTool, refrerenced from LArCellCont.
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge(addFolders(flags, ['/LAR/BadChannels/BadChannels'], 'LAR'))
    acc.merge(addFolders(flags, ['/LAR/BadChannels/MissingFEBs'], 'LAR'))

    from TileConditions.TileConditionsConfig import tileCondCfg
    acc.merge( tileCondCfg (flags) )

    from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
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
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Input.isMC=False
    ConfigFlags.lock()
    acc = ComponentAccumulator()
    
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    acc.merge( TrigBSReadCfg( ConfigFlags ) )

    acc.merge( trigCaloDataAccessSvcCfg( ConfigFlags ) )
    
    from TrigT2CaloCommon.TrigT2CaloCommonConf import TestCaloDataAccess
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

    #acc.getService("MessageSvc").Format = "% F%60W%S%7W%R%T %0W%M"
    #acc.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"

    acc.printConfig(True)

    #print acc.getService("TrigCaloDataAccessSvc")
    print acc.getPublicTool("LArRoI_Map")

    print "running this configuration"
    of = open("test.pkl", "w")
    acc.store(of)
    of.close()


    
    
