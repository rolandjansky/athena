
def enableJGTowerMaker( SuperCellContainer="SCell", ApplySCQual=True, SCBitMask=0x200 ):

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TrigT1CaloFexSim.EnableJGTowerMaker' )

    log.info("Enabling JGTowerMaker. It is setup to read SuperCellContainer %s and %sapply SCQuality" % (SuperCellContainer, "" if ApplySCQual else "not " ) )

    global svcMgr

    # Details for tower builder 
    # svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR_W:"
    svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=gJTowerMap.db;dbname=OFLP200"
    svcMgr += CfgMgr.THistSvc()
    # svcMgr.THistSvc.Output += ["OUTPUT DATAFILE='myOutputFile.root' OPT='RECREATE'"]

    # not sure about this as these variables are not used anywhere
    folderlist = [ "/LAR/Identifier/GTowerSCMap",
                   "/LAR/Identifier/JTowerSCMap"
                   ]
    folderspec = [ ("GTowerSCMap#GTowerSCMapAtlas#%s" % f) for f in folderlist ]
    tagspec    = [ ("".join(f.split("/"))+"-boohoo") for f in folderlist ]


    from TrigT1CaloFexSim.TrigT1CaloFexSimConf import JGTowerMaker

    algseq = CfgMgr.AthSequencer("AthAlgSeq")
    algseq += CfgMgr.JGTowerMaker( useSCQuality = ApplySCQual,
                                   useAllCalo = False,
                                   SuperCellType = SuperCellContainer,
                                   SuperCellQuality = SCBitMask
                                   )

    #I've removed CaloCellContainer#SCellBCID CaloCellContainer#SimpleSCell to only save one for now (Ben)
    '''
    StreamAOD.ItemList += [ "CaloCellContainer#AllCalo",
                            "xAOD::TrigEMClusterContainer#SClusterSimple",
                            "xAOD::TrigEMClusterAuxContainer#SClusterSimpleAux."]    
    StreamAOD.ItemList += [ "xAOD::TriggerTowerContainer#xAODTriggerTowers",
                            "xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux."]#Write out Run 2 towers 
    StreamAOD.ItemList += [ "xAOD::JGTowerContainer#JTower"]          # This and the next 3 might need to be controlled later to avoid increase of AOD size
    StreamAOD.ItemList += [ "xAOD::JGTowerAuxContainer#JTowerAux.*"]
    StreamAOD.ItemList += [ "xAOD::JGTowerContainer#GTower"]
    StreamAOD.ItemList += [ "xAOD::JGTowerAuxContainer#GTowerAux.*"]
    '''

