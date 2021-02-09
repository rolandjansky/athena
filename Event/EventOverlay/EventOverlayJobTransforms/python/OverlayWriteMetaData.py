# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'OverlayWriteMetadata' )


def writeOverlayDigitizationMetadata(pileupDict):
    from IOVDbMetaDataTools import ParameterDbFiller
    dbFiller = ParameterDbFiller.ParameterDbFiller()
    from Digitization.DigitizationFlags import digitizationFlags
    runNumber = digitizationFlags.dataRunNumber()
    log.debug('ParameterDbFiller BeginRun = %s', str(runNumber) )
    dbFiller.setBeginRun(runNumber)
    log.debug('ParameterDbFiller EndRun   = %s', str(runNumber+1) )
    dbFiller.setEndRun(runNumber+1)

    log.info('Filling Digitization MetaData')
    
    # Copy over pileup dictionary
    for key in pileupDict:
        value = str(pileupDict[key])
        log.info('DigitizationMetaData: setting "%s" to be %s', key, value)
        if key in ["BeamIntensityPattern"]:
            dbFiller.addDigitParam64(key, value)
        else:
            dbFiller.addDigitParam(key, value)

    # Make the MetaData Db
    dbFiller.genDigitDb()


def loadOverlayDigitizationMetadata():
    folder = "/Digitization/Parameters"
    dbConnection = "sqlite://;schema=DigitParams.db;dbname=DIGPARAM"
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.IOVDbSvc.Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
    ServiceMgr.IOVDbSvc.FoldersToMetaData += [ folder ]
    ServiceMgr.IOVSvc.partialPreLoadData = True
