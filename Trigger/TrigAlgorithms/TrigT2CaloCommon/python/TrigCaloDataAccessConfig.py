# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArRoIMapCfg( flags ):
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

CaloDataAccessSvcDependencies = [('IRegSelLUTCondData', 'ConditionStore+RegSelLUTCondData_TTEM'), 
                                 ('IRegSelLUTCondData', 'ConditionStore+RegSelLUTCondData_TTHEC'), 
                                 ('IRegSelLUTCondData', 'ConditionStore+RegSelLUTCondData_TILE'), 
                                 ('IRegSelLUTCondData', 'ConditionStore+RegSelLUTCondData_FCALEM'), 
                                 ('IRegSelLUTCondData', 'ConditionStore+RegSelLUTCondData_FCALHAD')]

from functools import lru_cache
@lru_cache(None)
def trigCaloDataAccessSvcCfg( flags ):    

    acc = ComponentAccumulator()
    svc = CompFactory.TrigCaloDataAccessSvc()

    # since the svc depends on calo geometry configure it here
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge( LArGMCfg( flags ) )

    from TileGeoModel.TileGMConfig import TileGMCfg    
    acc.merge( TileGMCfg( flags ) )
    
    acc.merge( LArRoIMapCfg( flags ) )

    #setup region selector
    from RegionSelector.RegSelToolConfig import (regSelTool_TTEM_Cfg,regSelTool_TTHEC_Cfg,
                                                 regSelTool_FCALEM_Cfg,regSelTool_FCALHAD_Cfg,regSelTool_TILE_Cfg)

    svc.RegSelToolEM = acc.popToolsAndMerge(regSelTool_TTEM_Cfg(flags))
    svc.RegSelToolHEC = acc.popToolsAndMerge(regSelTool_TTHEC_Cfg(flags))
    svc.RegSelToolFCALEM = acc.popToolsAndMerge(regSelTool_FCALEM_Cfg(flags))
    svc.RegSelToolFCALHAD = acc.popToolsAndMerge(regSelTool_FCALHAD_Cfg(flags))
    svc.RegSelToolTILE = acc.popToolsAndMerge(regSelTool_TILE_Cfg(flags))


    # Needed by bad channel maskers, refrerenced from LArCellCont.
    from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg, LArBadFebCfg
    acc.merge(LArBadChannelCfg(flags))
    acc.merge(LArBadFebCfg(flags))

    from TileConditions.TileEMScaleConfig import TileEMScaleCondAlgCfg
    acc.merge( TileEMScaleCondAlgCfg(flags) )

    from TileConditions.TileBadChannelsConfig import TileBadChannelsCondAlgCfg
    acc.merge( TileBadChannelsCondAlgCfg(flags) )
    
# TODO - complete this with appropriate conditions alg setup
#    from CaloRec.CaloBCIDAvgAlgConfig import CaloBCIDAvgAlgCfg
#    eventAcc = ComponentAccumulator("HLTBeginSeq")
#    eventAcc.merge(CaloBCIDAvgAlgCfg(flags), sequenceName="HLTBeginSeq")
#    acc.merge(eventAcc)

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
    acc.addSequence(CompFactory.AthSequencer("HLTBeginSeq"))
    
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge( ByteStreamReadCfg( ConfigFlags ) )

    acc.merge( trigCaloDataAccessSvcCfg( ConfigFlags ) )
    
    TestCaloDataAccess=CompFactory.TestCaloDataAccess
    testAlg = TestCaloDataAccess()
    acc.addEventAlgo(testAlg)    
    
    acc.printConfig(True)

    print(acc.getPublicTool("LArRoI_Map"))

    print("running this configuration")
    of = open("test.pkl", "wb")
    acc.store(of)
    of.close()


    
    
