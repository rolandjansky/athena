# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def LArCellDeadOTXCorrToolDefault(name='LArCellDeadOTXCorr'):
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'LArCellDeadOTXCorrToolDefault' )

    try :
        from LArCellRec.LArCellRecConf import LArCellDeadOTXCorr
        theLArCellDeadOTXCorr = LArCellDeadOTXCorr(name)
    except Exception:
        import traceback
        mlog.error("could not get handle to LArCellDeadOTXCorr Quit")
        traceback.print_exc()
        return False 
    from AthenaCommon.GlobalFlags import globalflags

    ignoredTTs = []
    deadOTXCorrDbFolder = "/LAR/CellCorrOfl/deadOTX"
    if globalflags.DataSource()=='data' :
        #deadOTXCorrDbTag = "<tag>LARCellCorrOflDeadOTX-UPD1-00</tag>"
        #deadOTXCorrDbConnection = "<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=COMP200</dbConnection>" 
        ignoredTTs = [0x11d0101]
        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType() == "cosmics" :
            theLArCellDeadOTXCorr.useL1CaloDB = False
        else :
            theLArCellDeadOTXCorr.useL1CaloDB = True
    else : 
        #deadOTXCorrDbTag = "<tag>LARCellCorrOflDeadOTX-000-00</tag>"
        #deadOTXCorrDbConnection = "<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=OFLP200</dbConnection>"
        theLArCellDeadOTXCorr.useL1CaloDB = False


    theLArCellDeadOTXCorr.triggerNoiseCut = [2.,0.265,0.45,0.16,0.87]
    theLArCellDeadOTXCorr.ignoredTTs = ignoredTTs
    theLArCellDeadOTXCorr.DBHandleKey = "deadOTXCorrCtes"

    from IOVDbSvc.CondDB import conddb
    #conddb.addFolder("", deadOTXCorrDbConnection + deadOTXCorrDbFolder + deadOTXCorrDbTag) 
    conddb.addFolder("LAR_OFL",deadOTXCorrDbFolder,
                     className = 'CondAttrListCollection')

    from CaloRec.ToolConstantsCondAlgDefault import ToolConstantsCondAlgDefault
    ToolConstantsCondAlgDefault ('deadOTXCorrCtes',
                                 COOLFolder = deadOTXCorrDbFolder )


    if theLArCellDeadOTXCorr.useL1CaloDB : 
        # Setup the L1CaloCondSvc
        from AthenaCommon.AppMgr import ServiceMgr
        from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
        ServiceMgr += L1CaloCondSvc()


        L1CaloDb=""
        if 'L1CaloDbConnection' not in dir():
            if 'L1CaloSqliteDB' in dir():
                L1CaloDbConnection="<dbConnection>sqlite://;schema=" + L1CaloSqliteDB + ";dbname=L1CALO</dbConnection>" # noqa: F821
            else:
                L1CaloDb="TRIGGER"
                L1CaloDbConnection=""


        L1CaloDbTag = "<tag>HEAD</tag>"


        L1CaloFolderList = []
        L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"]


        for l1calofolder in L1CaloFolderList:
            if not conddb.folderRequested(l1calofolder):
                conddb.addFolder(L1CaloDb, L1CaloDbConnection + l1calofolder + L1CaloDbTag)



    return theLArCellDeadOTXCorr


