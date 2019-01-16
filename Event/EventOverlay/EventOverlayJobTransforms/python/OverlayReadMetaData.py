# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logOverlayPoolReadMetadata = logging.getLogger( 'OverlayPoolReadMetadata' )

def hitColls2SimulatedDetectors(inputlist):
    """Build a dictionary from the list of containers in the metadata"""
    simulatedDetectors = []
    simulatedDictionary = {'PixelHits': 'pixel', 'SCT_Hits': 'SCT', 'TRTUncompressedHits': 'TRT',
                           'BCMHits': 'BCM', 'LucidSimHitsVector': 'Lucid', 'LArHitEMB': 'LAr',
                           'LArHitEMEC': 'LAr', 'LArHitFCAL': 'LAr', 'LArHitHEC': 'LAr',
                           'MBTSHits': 'Tile', 'TileHitVec': 'Tile', 'MDT_Hits': 'MDT',
                           'MicromegasSensitiveDetector': 'Micromegas',
                           'sTGCSensitiveDetector': 'sTGC',
                           'CSC_Hits': 'CSC', 'TGC_Hits': 'TGC', 'RPC_Hits': 'RPC',
                           'TruthEvent': 'Truth'} #'': 'ALFA', '': 'ZDC',
    for entry in inputlist:
        if entry[1] in simulatedDictionary.keys():
            if simulatedDictionary[entry[1]] not in simulatedDetectors:
                simulatedDetectors += [simulatedDictionary[entry[1]]]
    return simulatedDetectors

def checkTileCalibrationHitFormat(inputlist):
    """Check the Tile CaloCalibrationHit format"""
    oldnames = ["TileCalibrationCellHitCnt","TileCalibrationDMHitCnt"]
    newnames = ["TileCalibHitActiveCell","TileCalibHitInactiveCell","TileCalibHitDeadMaterial"]
    nold = 0
    nnew = 0
    for entry in inputlist:
        if entry[1] in oldnames:
            logOverlayPoolReadMetadata.debug("found %s in oldnames", entry[1])
            nold+=1
        if entry[1] in newnames:
            logOverlayPoolReadMetadata.debug("found %s in newnames", entry[1])
            nnew+=1
    if nold > 0 and nnew > 0:
        raise SystemExit("Input file contains both old and new style TileCaloCalibrationHit containers, please check your g4sim job.")
    elif nold > 0:
        from Digitization.DigitizationFlags import digitizationFlags
        digitizationFlags.experimentalDigi += ['OldTileCalibHitContainers']
        logOverlayPoolReadMetadata.info("Input file uses old TileCalibHitContainers names: %s", oldnames);
    elif nnew > 0:
        logOverlayPoolReadMetadata.info("Input file uses new TileCalibHitContainers names: %s", newnames);
    return

def listOptionalContainers(inputlist):
    """Generate a list of optional containers"""
    supported = ['TrackRecordCollection', 'CaloCalibrationHitContainer', 'HijingEventParams']

    containers = {}
    for entry in inputlist:
        if entry[0] not in supported:
            continue

        if entry[0] not in containers:
            containers[entry[0]] = set()

        containers[entry[0]].add(entry[1])

    return containers

## Helper functions
def skipCheck(key):
    """This check is not required"""
    from Digitization.DigitizationFlags import digitizationFlags
    if key in digitizationFlags.overrideMetadata.get_Value():
        return True
    return False

def skipPileUpCheck(key, pileuptype):
    """This check is not required"""
    if skipCheck(key):
        return True
    pileupkey='%s_%s' %(pileuptype,key)
    return skipCheck(pileupkey)

def doMC_channel_number(f,pileUpType):
    print "doMC_channel_number for %s", pileUpType
    if "mc_channel_number" in f.infos.keys():
        params = dict()
        from Digitization.DigitizationFlags import digitizationFlags
        if digitizationFlags.pileupDSID.statusOn:
            params = digitizationFlags.pileupDSID.get_Value()
        print "MC channel number from AthFile %s", f.infos["mc_channel_number"]
        params[pileUpType]= f.infos["mc_channel_number"]
        digitizationFlags.pileupDSID = params
        del params
    return

def doSpecialConfiguration(f):
    #safety checks before trying to access metadata
    if "tag_info" in f.infos.keys():
        if "specialConfiguration" in f.infos["tag_info"]:
            item = f.infos["tag_info"]["specialConfiguration"]
            logOverlayPoolReadMetadata.info("specialConfiguration directive: %s" % item)
            spcitems = item.split(";")
            preIncludes=[]
            params = {}
            from Digitization.DigitizationFlags import digitizationFlags
            if digitizationFlags.specialConfiguration.statusOn:
                logOverlayPoolReadMetadata.info("some spcialConfiguration metadata already exists: %s", str(params))
                params = digitizationFlags.specialConfiguration.get_Value()
            for spcitem in spcitems:
                ## Ignore empty (e.g. from consecutive or trailing semicolons) or "NONE" substrings
                if not spcitem or spcitem == "NONE":
                    continue
                ## If not in key=value format, treat as v, with k="preInclude"
                if "=" not in spcitem:
                    spcitem = "preInclude=" + spcitem
                ## Handle k=v directives
                k, v = spcitem.split("=")
                logOverlayPoolReadMetadata.info("specialConfiguration metadata item: %s => %s" % (k, v))
                ## Store preIncludes for including later.
                if k == "preInclude":
                    incfiles = v.split(",")
                    preIncludes+=incfiles
                else:
                    params[k] = v
            digitizationFlags.specialConfiguration = params
            ## Now that we've looked at and stored all the evgen metadata keys, we should do any requested preIncludes
            from AthenaCommon.Include import include
            for incfile in preIncludes:
                logOverlayPoolReadMetadata.info("Including %s as instructed by specialConfiguration metadata" % incfile)
                include(incfile)
            del preIncludes
            del params

def buildDict(inputtype, inputfile):
    """Build a dictionary of KEY:VALUE pairs"""
    import re
    import PyUtils.AthFile as af
    try:
        f = af.fopen(inputfile)
    except AssertionError:
        logOverlayPoolReadMetadata.error("Failed to open input file: %s", inputfile)
        return dict(),False
    #check evt_type of input file
    if 'evt_type' in f.infos.keys():
        if not re.match(str(f.infos['evt_type'][0]), 'IS_SIMULATION') :
            logOverlayPoolReadMetadata.error('This input file has incorrect evt_type: %s',str(f.infos['evt_type']))
            logOverlayPoolReadMetadata.info('Please make sure you have set input file metadata correctly.')
            logOverlayPoolReadMetadata.info('Consider using the job transforms for earlier steps if you aren\'t already.')
            #then exit gracefully
            raise SystemExit("Input file evt_type is incorrect, please check your g4sim and evgen jobs.")
    else :
        logOverlayPoolReadMetadata.warning('Could not find \'evt_type\' key in athfile.infos. Unable to that check evt_type is correct.')

    ## Not part of building the metadata dictionary, but this is the
    ## most convenient time to access this information.
    doSpecialConfiguration(f)
    #doMC_channel_number(f,inputtype) #FIXME commented out for now until mc_channel_number is filled properly by AthFile.

    metadatadict = dict()
    taginfometadata = dict()
    #safety checks before trying to access metadata
    if 'metadata' in f.infos.keys():
        ##if '/TagInfo' in f.infos['metadata'].keys():
        ##    taginfometadata=f.infos['metadata']['/TagInfo']
        ##    assert taginfometadata['beam_energy'] is not None
        ##    print "beamEnergy=%s"%taginfometadata['beam_energy']
        if '/Simulation/Parameters' in f.infos['metadata'].keys():
            metadatadict = f.infos['metadata']['/Simulation/Parameters']
            if isinstance(metadatadict, list):
                logOverlayPoolReadMetadata.warning("%s inputfile: %s contained %s sets of Simulation Metadata. Using the final set in the list.",inputtype,inputfile,len(metadatadict))
                metadatadict=metadatadict[-1]
        ##Get IOVDbGlobalTag
        if 'IOVDbGlobalTag' not in metadatadict.keys():
            try:
                assert f.fileinfos['metadata']['/TagInfo']['IOVDbGlobalTag'] is not None
                metadatadict['IOVDbGlobalTag'] = f.fileinfos['metadata']['/TagInfo']['IOVDbGlobalTag']
            except:
                try:
                    assert f.fileinfos['conditions_tag'] is not None
                    metadatadict['IOVDbGlobalTag'] = f.fileinfos['conditions_tag']
                except:
                    logOverlayPoolReadMetadata.warning("Failed to find IOVDbGlobalTag.")
                    return metadatadict,taginfometadata,False
        if '/Digitization/Parameters' in f.infos['metadata'].keys():
            ## We have the RDO file here:
            if len(f.run_numbers)>0 :
                logOverlayPoolReadMetadata.info("Setting digitizationFlags.dataRunNumber to %i, to match the pre-mixed RDO File", f.run_numbers[0])
                from Digitization.DigitizationFlags import digitizationFlags
                digitizationFlags.dataRunNumber = f.run_numbers[0]
            ##Do useful things with Digi MetaData
            digimetadatadict = f.infos['metadata']['/Digitization/Parameters']
            logOverlayPoolReadMetadata.debug("Keys in /Digitization/Parameters MetaData:")
            logOverlayPoolReadMetadata.debug(digimetadatadict.keys())

            if 'IOVDbGlobalTag' in digimetadatadict.keys():
                if 'IOVDbGlobalTag' in metadatadict.keys():
                    if not re.match(metadatadict['IOVDbGlobalTag'], digimetadatadict['IOVDbGlobalTag']):
                        logOverlayPoolReadMetadata.info("Overriding original SimDict['IOVDbGlobalTag']", metadatadict['IOVDbGlobalTag'])
                        logOverlayPoolReadMetadata.debug("with RDODict['IOVDbGlobalTag']", digimetadatadict['IOVDbGlobalTag'])
                        metadatadict['IOVDbGlobalTag'] = digimetadatadict['IOVDbGlobalTag']
                        logOverlayPoolReadMetadata.debug("Updated SimDict['IOVDbGlobalTag']", metadatadict['IOVDbGlobalTag'])
            if 'DigitizedDetectors' in digimetadatadict.keys():
                if 'DigitizedDetectors' in metadatadict.keys():
                    logOverlayPoolReadMetadata.debug("Overriding original SimDict['SimulatedDetectors']", metadatadict['SimulatedDetectors'])
                    logOverlayPoolReadMetadata.debug("with RDODict['DigitizedDetectors']", digimetadatadict['DigitizedDetectors'])
                    metadatadict['SimulatedDetectors'] = digimetadatadict['DigitizedDetectors']
                    logOverlayPoolReadMetadata.debug("Updated SimDict['SimulatedDetectors']", metadatadict['SimulatedDetectors'])
            pass
        if '/TagInfo' in f.infos['metadata'].keys():
            taginfometadata=f.infos['metadata']['/TagInfo']
            pass
    Nkvp = len(metadatadict)
    ## Dictionary should not be empty
    if Nkvp==0 :
        logOverlayPoolReadMetadata.error("Found %s KEY:VALUE pairs in %s Simulation MetaData." , Nkvp,inputtype)
        return metadatadict,taginfometadata,False
    else:
        ##Patch for older hit files
        if 'hitFileMagicNumber' not in metadatadict.keys():
            metadatadict['hitFileMagicNumber'] = 0
            logOverlayPoolReadMetadata.debug("hitFileMagicNumber key missing from %s Simulation MetaData Dictionary. Adding dummy entry.",inputtype)
        if 'SimulatedDetectors' not in metadatadict.keys():
            if 'eventdata_items' in f.infos.keys():
                metadatadict['SimulatedDetectors'] = hitColls2SimulatedDetectors(f.infos['eventdata_items'])
            else :
                metadatadict['SimulatedDetectors'] = ['pixel','SCT','TRT','BCM','Lucid','LAr','Tile','MDT','CSC','TGC','RPC','Truth']
        ## Check whether we should use the old names for the Tile CaloCalibrationHit containers
        if 'eventdata_items' in f.infos.keys():
            checkTileCalibrationHitFormat(f.infos['eventdata_items'])
        else :
            from Digitization.DigitizationFlags import digitizationFlags
            digitizationFlags.experimentalDigi += ['OldTileCalibHitContainers']

        ## Generate optional containers list
        if 'eventdata_items' in f.infos.keys():
            metadatadict['OptionalContainers'] = listOptionalContainers(f.infos['eventdata_items'])
        else:
            metadatadict['OptionalContainers'] = {}

        ##End of Patch for older hit files
        logOverlayPoolReadMetadata.debug("%s Simulation MetaData Dictionary Successfully Created.",inputtype)
        logOverlayPoolReadMetadata.debug("Found %s KEY:VALUE pairs in %s Simulation MetaData." , Nkvp,inputtype)
        logOverlayPoolReadMetadata.debug("KEYS FOUND: %s", metadatadict.keys())
        return metadatadict,taginfometadata,True

def signalMetaDataCheck(metadatadict):
    import re
    simkeys = metadatadict.keys()
    logOverlayPoolReadMetadata.info("Checking Digitization properties against Signal Simulation MetaData...")
    ## Check the PhysicsList set agrees with that used in the simulation
    if not skipCheck('PhysicsList'):
        if 'PhysicsList' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            if re.match(metadatadict['PhysicsList'], digitizationFlags.physicsList.get_Value()):
                logOverlayPoolReadMetadata.debug("Digitization properties matches Signal Simulation MetaData. [PhysicsList = %s]", metadatadict['PhysicsList'])
            else:
                logOverlayPoolReadMetadata.warning("Digitization properties PhysicsList does not match the PhysicsList used in the Simulation step! Assume the PhysicsList from the Simulation step is correct!")
                digitizationFlags.physicsList = metadatadict['PhysicsList']
                logOverlayPoolReadMetadata.info("Set digitizationFlags.physicsList = %s", digitizationFlags.physicsList.get_Value())
        else:
            logOverlayPoolReadMetadata.error("PhysicsList key not found in Simulation MetaData!")

    ## Check the DetDescrVersion set agrees with that used in the simulation
    if not skipCheck('SimLayout'):
        if 'SimLayout' in simkeys:
            from AthenaCommon.GlobalFlags import globalflags
            if re.match(metadatadict['SimLayout'], globalflags.DetDescrVersion.get_Value()):
                logOverlayPoolReadMetadata.debug("Digitization properties matches Signal Simulation MetaData. [DetDescrVersion = %s]",
                                            globalflags.DetDescrVersion.get_Value())
            else:
                logOverlayPoolReadMetadata.warning("Input DetDescrVersion does not match the value used in the Simulation step!")
                from AthenaCommon.AppMgr import ServiceMgr
                ## FIXME - should not be relying on GeoModelSvc being initialized at this point.
                if hasattr( ServiceMgr, "GeoModelSvc") and ServiceMgr.GeoModelSvc.IgnoreTagDifference==True:
                    logOverlayPoolReadMetadata.warning("Global jobproperties: [DetDescrVersion = %s], Signal Simulation MetaData: [SimLayout = %s]",
                                                  globalflags.DetDescrVersion.get_Value(), metadatadict['SimLayout'])
                    logOverlayPoolReadMetadata.warning("Ignore Tag Difference Requested - doing nothing.")
                else:
                    logOverlayPoolReadMetadata.warning("Assume the value from the Simulation step is correct!")
                    ## needs to be done this way as Digi_tf locks it
                    if globalflags.DetDescrVersion.is_locked() :
                        globalflags.DetDescrVersion.unlock()
                    globalflags.DetDescrVersion.set_Value_and_Lock( metadatadict['SimLayout'] )
                    logOverlayPoolReadMetadata.warning("Set globalflags.DetDescrVersion = %s",globalflags.DetDescrVersion.get_Value())
        else:
            logOverlayPoolReadMetadata.error("SimLayout key not found in Simulation MetaData!")

    ## Check the Conditions Tag set against that used in the simulation
    if not skipCheck('IOVDbGlobalTag'):
        if 'IOVDbGlobalTag' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            if (digitizationFlags.IOVDbGlobalTag.statusOn):
                logOverlayPoolReadMetadata.info("Digitization properties: [IOVDbGlobalTag = %s], Signal Simulation MetaData: [IOVDbGlobalTag = %s]",
                                           digitizationFlags.IOVDbGlobalTag.get_Value(), metadatadict['IOVDbGlobalTag'])
            else:
                digitizationFlags.IOVDbGlobalTag = metadatadict['IOVDbGlobalTag']
                logOverlayPoolReadMetadata.debug("Set Digitization properties to match Signal Simulation Metadata: [IOVDbGlobalTag = %s]",
                                            digitizationFlags.IOVDbGlobalTag.get_Value())
        else:
            logOverlayPoolReadMetadata.error("IOVDbGlobalTag key not found in Simulation MetaData!")

    ## Set the TRTRangeCut digitizationFlag based on what was used during the simulation.
    if not skipCheck('TRTRangeCut'):
        if 'TRTRangeCut' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            if hasattr( digitizationFlags, 'TRTRangeCut'):
                digitizationFlags.TRTRangeCut = metadatadict['TRTRangeCut']
                logOverlayPoolReadMetadata.debug("Set Digitization properties to match Signal Simulation Metadata: [TRTRangeCut = %s]",
                                            digitizationFlags.TRTRangeCut.get_Value())
        else:
            logOverlayPoolReadMetadata.warning("TRTRangeCut key not found in Simulation MetaData!")

    ## Record the G4Version used in the simulation, so that Digitization Algorithms can use this information
    if not skipCheck('G4Version'):
        if 'G4Version' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            digitizationFlags.SimG4VersionUsed = metadatadict['G4Version']
            logOverlayPoolReadMetadata.debug("digitizationFlags.SimG4VersionUsed = Value from Sim Metadata = %s ", digitizationFlags.SimG4VersionUsed.get_Value())
        else:
            logOverlayPoolReadMetadata.error("G4Version key not found in Simulation MetaData!")

    ## Check which sub-detectors were simulated

    ## Digitization will only digitize detectors which have been simulated.
    ## If users want to digitize an un simulated detector this will be an expert
    ## action which will require hacking the python code.
    if not skipCheck('SimulatedDetectors'):
        if 'SimulatedDetectors' in simkeys:
            from AthenaCommon.DetFlags import DetFlags
            logOverlayPoolReadMetadata.debug("Switching off subdetectors which were not simulated")
            possibleSubDetectors=['pixel','SCT','TRT','BCM','Lucid','ZDC','ALFA','AFP','FwdRegion','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth']
            switchedOffSubDetectors=[]
            for subdet in possibleSubDetectors:
                if not subdet in metadatadict['SimulatedDetectors']:
                    attrname = subdet+"_setOff"
                    checkfn = getattr(DetFlags, attrname, None)
                    if checkfn is not None:
                        cmd='DetFlags.%s_setOff()' % subdet
                        logOverlayPoolReadMetadata.debug(cmd)
                        checkfn()
                    switchedOffSubDetectors+=[subdet]
            if switchedOffSubDetectors:
                logOverlayPoolReadMetadata.info("Ensured %s sub-detectors which were not simulated were switched off: %s", len(switchedOffSubDetectors), switchedOffSubDetectors)
            else:
                logOverlayPoolReadMetadata.info("All sub-detectors were simulated, so none needed to be switched off in digitization.")
            DetFlags.Print()

    # Check for optional containers presence
    if not skipCheck('OptionalContainers'):
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        overlayFlags.optionalContainerMap.set_Value_and_Lock(metadatadict['OptionalContainers'])

    ## Any other checks here
    logOverlayPoolReadMetadata.info("Completed checks of Digitization properties against Signal Simulation MetaData.")

def pileupMetaDataCheck(sigsimdict,pileupsimdict):
    """Check the metadata for pre-mixed pileup RDO File"""
    result = True
    import re
    pileupkeys = pileupsimdict.keys()
    sigkeys = sigsimdict.keys()
    pileuptype = "PreMixed"
    longpileuptype = "Pre-Mixed Pile-up RDO File"
    ##Loop over MetaData keys which must have matching values
    SigkeysToCheck = [ 'PhysicsList', 'SimLayout', 'MagneticField','hitFileMagicNumber' ]#, 'WorldZRange' ]
    for o in SigkeysToCheck:
        if skipPileUpCheck(o, pileuptype):
            continue
        try:
            assert o in pileupkeys
        except AssertionError:
            logOverlayPoolReadMetadata.error("%s key missing from %s Simulation MetaData!", o, longpileuptype)
            raise AssertionError("Simulation MetaData key not found")
        try:
            assert o in sigkeys
        except AssertionError:
            logOverlayPoolReadMetadata.error("%s key missing from Signal Simulation MetaData!", o)
            raise AssertionError("Simulation MetaData key not found")
        try:
            if not isinstance(pileupsimdict[o],type(sigsimdict[o])):
                assert re.match(str(pileupsimdict[o]), str(sigsimdict[o]))
            else:
                if isinstance(pileupsimdict[o],str):
                    assert re.match(pileupsimdict[o], sigsimdict[o])
                elif isinstance(pileupsimdict[o],int):
                    assert (pileupsimdict[o]==sigsimdict[o])
                else:
                    assert re.match(str(pileupsimdict[o]), str(sigsimdict[o]))
        except AssertionError:
            logOverlayPoolReadMetadata.error("Simulation MetaData mismatch! %s: [%s = %s] Signal: [%s = %s]", longpileuptype, o, pileupsimdict[o], o, sigsimdict[o])
            raise AssertionError("Simulation MetaData mismatch")
        logOverlayPoolReadMetadata.debug("%s Sim MetaData matches Signal Sim MetaData. [%s = %s]", longpileuptype, o, sigsimdict[o])
    ##Ideally these keys would have matching values, but it should be OK if not.
    WarningKeys = [ 'IOVDbGlobalTag', 'G4Version' ]
    for o in WarningKeys:
        if skipPileUpCheck(o, pileuptype):
            continue
        try:
            assert o in pileupkeys
        except AssertionError:
            logOverlayPoolReadMetadata.error("%s key missing from %s Simulation MetaData!", o, longpileuptype)
            raise AssertionError("Simulation MetaData key not found")
        try:
            assert o in sigkeys
        except AssertionError:
            logOverlayPoolReadMetadata.error("%s key missing from Signal Simulation MetaData!", o, longpileuptype)
            raise AssertionError("Simulation MetaData key not found")
        if not re.match(pileupsimdict[o], sigsimdict[o]):
            logOverlayPoolReadMetadata.warning("Simulation MetaData mismatch! %s: [%s = %s] Signal: [%s = %s]", longpileuptype, o, pileupsimdict[o], o, sigsimdict[o])
        else:
            logOverlayPoolReadMetadata.debug("%s Sim MetaData matches Signal Sim MetaData. [%s = %s]", longpileuptype, o, sigsimdict[o])

    ## Check that the same sub-detectors were simulated in signal and background inputs
    if (not skipPileUpCheck('SimulatedDetectors', pileuptype)) and ('SimulatedDetectors' in sigkeys):
        switchedOffSubDetectors=[]
        for subdet in sigsimdict['SimulatedDetectors']:
            if not subdet in pileupsimdict['SimulatedDetectors']:
                switchedOffSubDetectors+=[subdet]
        if switchedOffSubDetectors:
            logOverlayPoolReadMetadata.error("%s sub-detectors were sinmulated in the signal sample, but not in the %s background sample: %s", len(switchedOffSubDetectors), longpileuptype, switchedOffSubDetectors)
            raise AssertionError("Some simulated sub-detectors from signal sample are missing in the background samples.")
        else:
            logOverlayPoolReadMetadata.debug("All sub-detectors simulated in the signal sample were also simulated in the %s background sample.", longpileuptype)

    return result


def tagInfoMetaDataCheck(sigtaginfodict,pileuptaginfodict):
    result = True
    """Check the metadata for pre-mixed pileup RDO File"""
    pileupkeys = pileuptaginfodict.keys()
    logOverlayPoolReadMetadata.debug("Signal /TagInfo ", sigtaginfodict)
    logOverlayPoolReadMetadata.debug("Pileup /TagInfo ", pileuptaginfodict)
    sigkeys = sigtaginfodict.keys()
    pileupType = "PreMixed"
    sigOnlyDict = dict()
    sigOnlyKeySet = set(sigkeys).difference(set(pileupkeys))
    logOverlayPoolReadMetadata.debug("The following keys only appear in Signal /TagInfo MetaData:")
    logOverlayPoolReadMetadata.debug(sigOnlyKeySet)
    for key in sigOnlyKeySet:
        sigOnlyDict[key] = sigtaginfodict[key]
        logOverlayPoolReadMetadata.debug("key: ", key, "value: ", sigtaginfodict[key])
        pass
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    overlayFlags.extraTagInfoPairs = sigOnlyDict
    keysToCompareSet = set(sigkeys).intersection(set(pileupkeys))
    logOverlayPoolReadMetadata.debug("The following keys appear in Signal and PreMixed /TagInfo MetaData:")
    logOverlayPoolReadMetadata.debug(keysToCompareSet)
    return result

def readInputFileMetadata():
    logOverlayPoolReadMetadata.info("Checking for Signal Simulation MetaData...")
    import re
    import PyUtils.AthFile as af
    af.server.load_cache('digitization-afcache.ascii')

    #--------------------------------------------------
    # Check for the Run Number in the first Input file
    #--------------------------------------------------
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from Digitization.DigitizationFlags import digitizationFlags
    digitizationFlags.simRunNumber = int(digitizationFlags.getHitFileRunNumber(athenaCommonFlags.PoolHitsInput.get_Value()[0]))

    sigsimdict,sigtaginfodict,result = buildDict("Signal", athenaCommonFlags.PoolHitsInput.get_Value()[0])
    if result :
        signalMetaDataCheck(sigsimdict)

        ## Check Pileup Simulation Parameters match those used for signal files
        result = True
        longpileuptype= "pre-mixed pile-up"
        logOverlayPoolReadMetadata.info("Checking %s MetaData against Signal Simulation MetaData...", longpileuptype)
        pileupsimdict,pileuptaginfodict,result = buildDict(longpileuptype, athenaCommonFlags.PoolRDOInput.get_Value()[0])
        if not result:
            logOverlayPoolReadMetadata.warning("Failed to Create %s Simulation MetaData Dictionary from file %s.", longpileuptype, pileupfile)
        else:
            if pileupMetaDataCheck(sigsimdict,pileupsimdict):
                logOverlayPoolReadMetadata.info("Pre-mixed RDO File Simulation MetaData matches Signal Simulation MetaData.")
            if tagInfoMetaDataCheck(sigtaginfodict,pileuptaginfodict):
                logOverlayPoolReadMetadata.info("Pre-mixed RDO File TagInfo MetaData matches Signal TagInfo MetaData.")
        ## All checks completed here
        logOverlayPoolReadMetadata.info("Completed all checks against Signal Simulation MetaData.")
    else:
        logOverlayPoolReadMetadata.info("Failed to Create Signal MetaData Dictionaries from file %s", athenaCommonFlags.PoolHitsInput.get_Value()[0])

    ## control where metadata can be used
    del sigsimdict
    del sigtaginfodict
    del pileupsimdict
    del pileuptaginfodict

    #raise SystemExit("Testing")
