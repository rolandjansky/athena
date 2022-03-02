# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaKernel.EventIdOverrideConfig import getMinMaxRunNumbers
from AthenaCommon.Logging import logging
from AthenaConfiguration.Enums import FlagEnum
logDigitizationWriteMetadata = logging.getLogger( 'DigitizationParametersConfig' )

def writeDigitizationMetadata(ConfigFlags):
    from IOVDbMetaDataTools import ParameterDbFiller
    dbFiller = ParameterDbFiller.ParameterDbFiller()
    myRunNumber, myEndRunNumber = getMinMaxRunNumbers(ConfigFlags)
    logDigitizationWriteMetadata.debug('ParameterDbFiller BeginRun = %s', str(myRunNumber) )
    dbFiller.setBeginRun(myRunNumber)
    logDigitizationWriteMetadata.debug('ParameterDbFiller EndRun   = %s', str(myEndRunNumber) )
    dbFiller.setEndRun(myEndRunNumber)
    #-------------------------------------------------
    # Adding jobproperties to the list of MetaData
    #-------------------------------------------------
    # Here list the digitization jobproperties we want to write out as MetaData.
    digitMetaDataKeys = { 'doInDetNoise' : 'Digitization.DoInnerDetectorNoise',
                          'doCaloNoise' : 'Digitization.DoCaloNoise',
                          'doMuonNoise' : "Digitization.DoMuonNoise",
                          'bunchSpacing' : 'Beam.BunchSpacing',
                          'beamType' : 'Beam.Type',
                          'IOVDbGlobalTag' : 'IOVDb.GlobalTag',
                          'DetDescrVersion' : 'GeoModel.AtlasVersion'
                      }
    logDigitizationWriteMetadata.info('Filling Digitization MetaData')
    for testKey, testFlag in digitMetaDataKeys.items():
        if ConfigFlags.hasFlag(testFlag):
            testValue = ConfigFlags._get(testFlag)
            if isinstance(testValue, FlagEnum):
                testValue = testValue.value
            if not isinstance(testValue, str):
                testValue = str(testValue)
            dbFiller.addDigitParam(testKey, testValue)
            logDigitizationWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)
        else :
            logDigitizationWriteMetadata.debug('DigitizationMetaData:  ConfigFlags.%s is not available.', testFlag)
    del digitMetaDataKeys

    # Bunch Structure - hardcoded for now
    testKey = "BeamIntensityPattern"
    testPattern = None # This should be replaced by a ConfigFlag
    testValue = str(testPattern)
    logDigitizationWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)
    dbFiller.addDigitParam64(testKey, testValue)

    # intraTrainBunchSpacing - hardcoded for now
    testKey = "intraTrainBunchSpacing"
    testValue = str(25) # This should be either be determined from the BeamIntensityPattern or set as ConfigFlag
    dbFiller.addDigitParam(testKey, testValue)
    logDigitizationWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)

    ## Digitized detector flags: add each enabled detector to the DigitizedDetectors list - might be better to determine this from the OutputStream or CA-itself? Possibly redundant info though?
    digiDets = []
    for det in ['Pixel','SCT','TRT','BCM','Lucid','ZDC','ALFA','AFP','FwdRegion','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','MM','sTGC','Truth','LVL1','ITkPixel','ITkStrip']:
        attrname = "Detector.Enable"+det
        if ConfigFlags.hasFlag(attrname):
            testValue = ConfigFlags._get(attrname)
            if testValue:
                digiDets.append(det)
        else:
            logDigitizationWriteMetadata.info("No flag called '%s' found in ConfigFlags", attrname)
    logDigitizationWriteMetadata.info("Setting 'DigitizedDetectors' = %s" , repr(digiDets))
    dbFiller.addDigitParam('DigitizedDetectors', repr(digiDets))

    #-------------------------------------------------
    # Make the MetaData Db
    #-------------------------------------------------
    dbFiller.genDigitDb()

    folder = "/Digitization/Parameters"
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    cfg=addFolders(ConfigFlags,[folder,],detDb="DigitParams.db",db="DIGPARAM",className='AthenaAttributeList')
    cfg.getService("IOVDbSvc").FoldersToMetaData += [ folder ]
    #cfg.getService("IOVSvc").partialPreLoadData = True #FIXME IOVSvc missing??
    return cfg

