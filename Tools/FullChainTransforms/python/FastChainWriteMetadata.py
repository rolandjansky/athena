# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logFastChainWriteMetadata = logging.getLogger( 'FastChainWriteMetadata' )
#-------------------------------------------------
# Setup MetaData Stream
#-------------------------------------------------
#if not hasattr(ServiceMgr, 'MetaDataStore'):
### In future should probably do a check here

#from AthenaServices.AthenaServicesConf import AthenaOutputStream
#StreamRDO_FH =  AthenaOutputStream( "StreamRDO_FH" )
#StreamRDO_FH.ItemList += [ "IOVMetaDataContainer#*" ]


myRunNumber = 0
myEndRunNumber = 2147483647 # the max run number (32 bit integer)

def ModifyingEventIdBySvc():
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.RunAndLumiOverrideList.statusOn or digitizationFlags.dataRunNumber.statusOn:
        from AthenaCommon.AppMgr import ServiceMgr
        if hasattr(ServiceMgr, 'EvtIdModifierSvc'):
            if hasattr(ServiceMgr.ToolSvc, 'IOVDbMetaDataTool'):
                return True
            else:
                logFastChainWriteMetadata.error('Found the EvtIdModifierSvc, but not the IOVDbMetaDataTool. \
                It will not be possible to set the min and max run number of the job.')
                raise SystemExit("Found the EvtIdModifierSvc, but not the IOVDbMetaDataTool.")
        else:
            raise SystemExit("jobProperties.Digitization.RunAndLumiOverrideList is set, but no EvtIdModifierSvc found!")
    else :
        logFastChainWriteMetadata.debug('digitizationFlags.RunAndLumiOverrideList not set, \
        so not overriding min/max run numbers.')
    return False

def getRunNumberRangeForOutputMetadata():
    myRunNumber = 0
    myEndRunNumber = 2147483647 # the max run number
    from Digitization.DigitizationFlags import digitizationFlags
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if not digitizationFlags.simRunNumber.statusOn and not athenaCommonFlags.DoFullChain:
        #---------------------------------------------------
        # Always Check for RunNumber in the first Input file
        #---------------------------------------------------
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        myRunNumber = digitizationFlags.getHitFileRunNumber(athenaCommonFlags.PoolHitsInput.get_Value()[0])

    if ModifyingEventIdBySvc():
        logFastChainWriteMetadata.info('Setting the  Digitization MetaData IOV from the IOVDbMetaDataTool')
        from AthenaCommon.AppMgr import ServiceMgr
        myRunNumber=ServiceMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers[0]
        myEndRunNumber=ServiceMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers[1]
    else :
        if myRunNumber > 0 :
            logFastChainWriteMetadata.info('Found Run Number %s in hits file metadata.', str(myRunNumber) )
            myEndRunNumber = myRunNumber+1 # got a reasonable run number so set end run to be the next run after this one.
        else :
            logFastChainWriteMetadata.info('Found unexpected Run Number %s in hits file metadata. Not overriding RunNumber to match hits file for this job.', str(myRunNumber) )
            myRunNumber = 0
    return myRunNumber, myEndRunNumber

def writeDigitizationMetadata():
    from IOVDbMetaDataTools import ParameterDbFiller
    dbFiller = ParameterDbFiller.ParameterDbFiller()
    myRunNumber, myEndRunNumber = getRunNumberRangeForOutputMetadata()
    logFastChainWriteMetadata.debug('ParameterDbFiller BeginRun = %s', str(myRunNumber) )
    dbFiller.setBeginRun(myRunNumber)
    logFastChainWriteMetadata.debug('ParameterDbFiller EndRun   = %s', str(myEndRunNumber) )
    dbFiller.setEndRun(myEndRunNumber)
    #-------------------------------------------------
    # Adding jobproperties to the list of MetaData
    #-------------------------------------------------
    # Here list the digitization jobproperties we want to write out as MetaData.
    digitMetaDataKeys = ["doInDetNoise", "doCaloNoise", "doMuonNoise", "bunchSpacing",
                         "initialBunchCrossing", "finalBunchCrossing", "FixedT0BunchCrossing",
                         "doLowPtMinimumBias", "doHighPtMinimumBias",
                         "doCavern", "doBeamGas", "doBeamHalo",
                         "rndmSvc", "physicsList", "overrideMetadata","pileupDSID","digiSteeringConf"]
                         #"readSeedsFromFile", "rndmSeedInputFile",
    from Digitization.DigitizationFlags import digitizationFlags
    if (not ModifyingEventIdBySvc()):
        # in run-dependent-conditions digitization jobs these values may vary between jobs with the same IOV
        digitMetaDataKeys += ["numberOfCollisions", "numberOfLowPtMinBias", "numberOfHighPtMinBias",
                              "numberOfCavern", "numberOfBeamGas", "numberOfBeamHalo",
                              "rndmSeedOffset1", "rndmSeedOffset2", "rndmSeedList"]
    digitPropertiesDict = digitizationFlags.__dict__
    digitPropertiesDictKeys = digitPropertiesDict.keys()
    logFastChainWriteMetadata.info('Filling Digitization MetaData')
    from Digitization.DigitizationFlags import digitizationFlags
    for o in [ o for o in digitMetaDataKeys if o in digitPropertiesDictKeys ]:
        testProperty = digitPropertiesDict.get(o)
        if testProperty.statusOn or 'bool' in testProperty.allowedTypes :
            testValue = testProperty.get_Value()
            if not isinstance(testValue, str):
                testValue = str(testProperty.get_Value())
            dbFiller.addDigitParam(o, testValue)
            logFastChainWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', o, testValue)
        else :
            logFastChainWriteMetadata.debug('DigitizationMetaData: Not using jobproperty "%s" as it is switched off.', o)
    del digitMetaDataKeys

    # Bunch Structure
    testKey = "BeamIntensityPattern"
    if digitizationFlags.BeamIntensityPattern.statusOn:
        testValue = str(digitizationFlags.BeamIntensityPattern.get_Value())
    else:
        testValue = "None"
    logFastChainWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)
    dbFiller.addDigitParam64(testKey, testValue)

    # BeamFlags
    testKey = "beamType"
    from AthenaCommon.BeamFlags import jobproperties
    testValue = jobproperties.Beam.beamType.get_Value()
    logFastChainWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)
    dbFiller.addDigitParam(testKey, testValue)
    # Use Beam.bunchSpacing to specify the most representative spacing (intra-train) between filled bunch-crossings.
    testKey = "intraTrainBunchSpacing"
    testValue = str(jobproperties.Beam.bunchSpacing.get_Value())
    logFastChainWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)
    dbFiller.addDigitParam(testKey, testValue)

    # IOVDbGlobalTag is a special case here:
    testKey = "IOVDbGlobalTag"
    testValue = "default"
    if digitizationFlags.IOVDbGlobalTag.statusOn :
        testValue = digitizationFlags.IOVDbGlobalTag.get_Value()
        if testValue == "" :
            testValue = "default"
    logFastChainWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)
    dbFiller.addDigitParam(testKey, testValue)

    ##these collections can get too large to write out in full, so instead we count the number of files.
    colsMetaDataKeys = ["LowPtMinBiasInputCols", "HighPtMinBiasInputCols",
                        "cavernInputCols", "beamGasInputCols", "beamHaloInputCols"]
    for o in [ o for o in colsMetaDataKeys if o in digitPropertiesDictKeys ]:
        testKey = 'N_' + o.replace('Cols','Files')
        testValue = '0'
        if digitPropertiesDict.get(o).statusOn :
            testList = digitPropertiesDict.get(o).get_Value()
            if isinstance(testList, list):
                testValue = str(len(testList))
            else :
                logFastChainWriteMetadata.warning('DigitizationMetaData: Key %s did not return a list. Ignoring.', o)
        logFastChainWriteMetadata.info('DigitizationMetaData: setting %s to be %s', testKey, testValue)
        dbFiller.addDigitParam(testKey, testValue)
    del colsMetaDataKeys

    ##this is also too long to write out in full. Just set flag if run-dependent MC is on.
    testKey = 'RunAndLumiAreOverriden'
    testValue = 'False'
    if digitPropertiesDict.get('RunAndLumiOverrideList').statusOn :
        testValue = 'True'
        logFastChainWriteMetadata.info('DigitizationMetaData: setting %s to be %s', testKey, testValue)
    dbFiller.addDigitParam(testKey, testValue)

    ##In case the value used for Digitization differs from that used for simulation.
    globalMetaDataKeys = ["DetDescrVersion"]
    from AthenaCommon.GlobalFlags import globalflags
    globalPropertiesDict = globalflags.__dict__
    for o in [ o for o in globalMetaDataKeys if o in globalPropertiesDict.keys() ]:
        testProperty = globalPropertiesDict.get(o)
        if testProperty.statusOn or 'bool' in testProperty.allowedTypes :
            testValue = testProperty.get_Value()
            if not isinstance(testValue, str):
                testValue = str(testProperty.get_Value())
            dbFiller.addDigitParam(o, testValue)
            logFastChainWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', o, testValue)
        else :
            logFastChainWriteMetadata.debug('DigitizationMetaData: Not using jobproperty "%s" as it is switched off.', o)
    del globalMetaDataKeys

    ##LVL1 Config Version
    testKey = "lvl1TriggerMenu"
    testValue = "NONE"
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.digitize.LVL1_on():
        #Protection so that TriggerFlags are not accessed when they have not be configured
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        testValue = TriggerFlags.triggerConfig.get_Value()
    dbFiller.addDigitParam(testKey,testValue)
    logFastChainWriteMetadata.info('DigitizationMetaData: setting "%s" to be %s', testKey, testValue)
    del testKey
    del testValue

    ## Digitized detector flags: add each enabled detector to the DigitizedDetectors list
    digiDets = []
    for det in ['pixel','SCT','TRT','BCM','Lucid','ZDC','ALFA','AFP','FwdRegion','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth','LVL1']:
        attrname = det+"_on"
        checkfn = getattr(DetFlags.digitize, attrname, None)
        if checkfn is None:
            logFastChainWriteMetadata.info("No attribute '%s' found on DetFlags.digitize" % attrname)
            continue
        if checkfn():
            digiDets.append(det)
    logFastChainWriteMetadata.info("Setting 'DigitizedDetectors' = %s" % repr(digiDets))
    dbFiller.addDigitParam('DigitizedDetectors', repr(digiDets))

    #-------------------------------------------------
    # Make the MetaData Db
    #-------------------------------------------------
    dbFiller.genDigitDb()

    folder = "/Digitization/Parameters"
    dbConnection = "sqlite://;schema=DigitParams.db;dbname=DIGPARAM"
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.IOVDbSvc.Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
    ServiceMgr.IOVDbSvc.FoldersToMetaData += [ folder ]
    ServiceMgr.IOVSvc.partialPreLoadData = True

    #raise SystemExit("Testing")


def createSimulationParametersMetadata():
    from IOVDbMetaDataTools import ParameterDbFiller
    dbFiller = ParameterDbFiller.ParameterDbFiller()
    myRunNumber, myEndRunNumber = getRunNumberRangeForOutputMetadata()
    logFastChainWriteMetadata.info("Using the following run number range for MetaData IOV: ("+str(myRunNumber)+","+str(myEndRunNumber)+").")
    dbFiller.setBeginRun(myRunNumber)
    dbFiller.setEndRun(myEndRunNumber)

    from ISF_Example.ISF_Metadata import fillAtlasMetadata, fillISFMetadata
    fillAtlasMetadata(dbFiller)
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.ISFRun:
        fillISFMetadata(dbFiller)

    ## Write the db info
    dbFiller.genSimDb()
    folder = "/Simulation/Parameters"
    dbConnection = "sqlite://;schema=SimParams.db;dbname=SIMPARAM"
    import IOVDbSvc.IOVDb
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.IOVDbSvc.Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
    ServiceMgr.IOVDbSvc.FoldersToMetaData += [folder]
    ServiceMgr.IOVSvc.partialPreLoadData = True

