# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def LArCellDeadOTXCorrToolDefault(name='LArCellDeadOTXCorr'):

    import traceback
    try :
        from LArCellRec.LArCellRecConf import LArCellDeadOTXCorr
        theLArCellDeadOTXCorr = LArCellDeadOTXCorr(name)
    except:
        mlog.error("could not get handle to LArCellDeadOTXCorr Quit")
        print traceback.format_exc()
        return False 
    try : 
        from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
        theLArBadChannelTool=LArBadChanTool()
    except:
        mlog.error("could not access bad channel tool Quit")
        print traceback.format_exc()
        return False 

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+=theLArBadChannelTool
    theLArCellDeadOTXCorr.badChannelTool = theLArBadChannelTool
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
    theLArCellDeadOTXCorr.COOLFolder = deadOTXCorrDbFolder 
    theLArCellDeadOTXCorr.detStoreKey = "deadOTXCorrCtes"

    from IOVDbSvc.CondDB import conddb
    #conddb.addFolder("", deadOTXCorrDbConnection + deadOTXCorrDbFolder + deadOTXCorrDbTag) 
    conddb.addFolder("LAR_OFL",deadOTXCorrDbFolder )




    if theLArCellDeadOTXCorr.useL1CaloDB : 
        # Setup the L1CaloCondSvc
        from AthenaCommon.AppMgr import ServiceMgr
        from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
        ServiceMgr += L1CaloCondSvc()


        L1CaloDb=""
        if not 'L1CaloDbConnection' in dir():
            if 'L1CaloSqliteDB' in dir():
                L1CaloDbConnection="<dbConnection>sqlite://;schema=" + L1CaloSqliteDB + ";dbname=L1CALO</dbConnection>"
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


