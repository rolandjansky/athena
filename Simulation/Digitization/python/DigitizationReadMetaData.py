# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logDigitizationReadMetadata = logging.getLogger( 'DigitizationReadMetadata' )

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
            logDigitizationReadMetadata.debug("found %s in oldnames", entry[1])
            nold+=1
        if entry[1] in newnames:
            logDigitizationReadMetadata.debug("found %s in newnames", entry[1])
            nnew+=1
    if nold > 0 and nnew > 0:
        raise SystemExit("Input file contains both old and new style TileCaloCalibrationHit containers, please check your g4sim job.")
    elif nold > 0:
        from Digitization.DigitizationFlags import digitizationFlags
        digitizationFlags.experimentalDigi += ['OldTileCalibHitContainers']
        logDigitizationReadMetadata.info("Input file uses old TileCalibHitContainers names: %s", oldnames);
    elif nnew > 0:
        logDigitizationReadMetadata.info("Input file uses new TileCalibHitContainers names: %s", newnames);
    return

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

def expandPileUpType(shortpileuptype):
    """convert the short background type names into the longer versions"""
    if shortpileuptype=="LowPtMinBias":
        return "Low Pt Minimum Bias"
    if shortpileuptype=="HighPtMinBias":
        return "High Pt Minimum Bias"
    if shortpileuptype=="cavern":
        return "Cavern Background"
    if shortpileuptype=="beamgas":
        return "Beam Gas"
    if shortpileuptype=="beamhalo":
        return "Beam Halo"
    return "Unknown Pile-Up Type"

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
            logDigitizationReadMetadata.info("specialConfiguration directive: %s" % item)
            spcitems = item.split(";")
            preIncludes=[]
            params = {}
            from Digitization.DigitizationFlags import digitizationFlags
            if digitizationFlags.specialConfiguration.statusOn:
                logDigitizationReadMetadata.info("some spcialConfiguration metadata already exists: %s", str(params))
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
                logDigitizationReadMetadata.info("specialConfiguration metadata item: %s => %s" % (k, v))
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
                logDigitizationReadMetadata.info("Including %s as instructed by specialConfiguration metadata" % incfile)
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
        logDigitizationReadMetadata.error("Failed to open input file: %s", inputfile)
        return dict(),False
    #check evt_type of input file
    if 'evt_type' in f.infos.keys():
        if not re.match(str(f.infos['evt_type'][0]), 'IS_SIMULATION') :
            logDigitizationReadMetadata.error('This input file has incorrect evt_type: %s',str(f.infos['evt_type']))
            logDigitizationReadMetadata.info('Please make sure you have set input file metadata correctly.')
            logDigitizationReadMetadata.info('Consider using the job transforms for earlier steps if you aren\'t already.')
            #then exit gracefully
            raise SystemExit("Input file evt_type is incorrect, please check your g4sim and evgen jobs.")
    else :
        logDigitizationReadMetadata.warning('Could not find \'evt_type\' key in athfile.infos. Unable to that check evt_type is correct.')

    ## Not part of building the metadata dictionary, but this is the
    ## most convenient time to access this information.
    doSpecialConfiguration(f)
    #doMC_channel_number(f,inputtype) #FIXME commented out for now until mc_channel_number is filled properly by AthFile.

    metadatadict = dict()
    #safety checks before trying to access metadata
    if 'metadata' in f.infos.keys():
        ##if '/TagInfo' in f.infos['metadata'].keys():
        ##    taginfometadata=f.infos['metadata']['/TagInfo']
        ##    assert taginfometadata['beam_energy'] is not None
        ##    print "beamEnergy=%s"%taginfometadata['beam_energy']
        if '/Simulation/Parameters' in f.infos['metadata'].keys():
            metadatadict = f.infos['metadata']['/Simulation/Parameters']
            if isinstance(metadatadict, list):
                logDigitizationReadMetadata.warning("%s inputfile: %s contained %s sets of Simulation Metadata. Using the final set in the list.",inputtype,inputfile,len(metadatadict))
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
                    logDigitizationReadMetadata.warning("Failed to find IOVDbGlobalTag.")
                    return metadatadict,False
    Nkvp = len(metadatadict)
    ## Dictionary should not be empty
    if Nkvp==0 :
        logDigitizationReadMetadata.error("Found %s KEY:VALUE pairs in %s Simulation MetaData." , Nkvp,inputtype)
        return metadatadict,False
    else:
        ##Patch for older hit files
        if 'hitFileMagicNumber' not in metadatadict.keys():
            metadatadict['hitFileMagicNumber'] = 0
            logDigitizationReadMetadata.debug("hitFileMagicNumber key missing from %s Simulation MetaData Dictionary. Adding dummy entry.",inputtype)
        if 'SimulatedDetectors' not in metadatadict.keys():
            if 'eventdata_items' in f.infos.keys():
                metadatadict['SimulatedDetectors'] = hitColls2SimulatedDetectors(f.infos['eventdata_items'])
            else :
                metadatadict['SimulatedDetectors'] = ['pixel','SCT','TRT','BCM','Lucid','LAr','Tile','MDT','CSC','TGC','RPC','Truth']
        ## Check whether we should use the old names for the Tile CaloCalibrationHit containers
        if 'eventdata_items' in f.infos.keys():
            checkTileCalibrationHitFormat(f.infos['eventdata_items'])
        else :
            digitizationFlags.experimentalDigi += ['OldTileCalibHitContainers']
        ##End of Patch for older hit files
        logDigitizationReadMetadata.debug("%s Simulation MetaData Dictionary Successfully Created.",inputtype)
        logDigitizationReadMetadata.debug("Found %s KEY:VALUE pairs in %s Simulation MetaData." , Nkvp,inputtype)
        logDigitizationReadMetadata.debug("KEYS FOUND: %s", metadatadict.keys())
        return metadatadict,True

def signalMetaDataCheck(metadatadict):
    import re
    simkeys = metadatadict.keys()
    logDigitizationReadMetadata.info("Checking Digitization properties against Signal Simulation MetaData...")
    ## Check the PhysicsList set agrees with that used in the simulation
    if not skipCheck('PhysicsList'):
        if 'PhysicsList' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            if re.match(metadatadict['PhysicsList'], digitizationFlags.physicsList.get_Value()):
                logDigitizationReadMetadata.debug("Digitization properties matches Signal Simulation MetaData. [PhysicsList = %s]", metadatadict['PhysicsList'])
            else:
                logDigitizationReadMetadata.warning("Digitization properties PhysicsList does not match the PhysicsList used in the Simulation step! Assume the PhysicsList from the Simulation step is correct!")
                digitizationFlags.physicsList = metadatadict['PhysicsList']
                logDigitizationReadMetadata.info("Set digitizationFlags.physicsList = %s", digitizationFlags.physicsList.get_Value())
        else:
            logDigitizationReadMetadata.error("PhysicsList key not found in Simulation MetaData!")

    ## Check the DetDescrVersion set agrees with that used in the simulation
    if not skipCheck('SimLayout'):
        if 'SimLayout' in simkeys:
            from AthenaCommon.GlobalFlags import globalflags
            if re.match(metadatadict['SimLayout'], globalflags.DetDescrVersion.get_Value()):
                logDigitizationReadMetadata.debug("Digitization properties matches Signal Simulation MetaData. [DetDescrVersion = %s]",
                                            globalflags.DetDescrVersion.get_Value())
            else:
                logDigitizationReadMetadata.warning("Input DetDescrVersion does not match the value used in the Simulation step!")
                from AthenaCommon.AppMgr import ServiceMgr
                ## FIXME - should not be relying on GeoModelSvc being initialized at this point.
                ignoreTagDifference = False
                try:
                    if hasattr( ServiceMgr, "GeoModelSvc") and ServiceMgr.GeoModelSvc.IgnoreTagDifference==True:
                        ignoreTagDifference = True
                except:
                    pass
                
                if ignoreTagDifference:
                    logDigitizationReadMetadata.warning("Global jobproperties: [DetDescrVersion = %s], Signal Simulation MetaData: [SimLayout = %s]",
                                                  globalflags.DetDescrVersion.get_Value(), metadatadict['SimLayout'])
                    logDigitizationReadMetadata.warning("Ignore Tag Difference Requested - doing nothing.")
                else:
                    logDigitizationReadMetadata.warning("Assume the value from the Simulation step is correct!")
                    ## needs to be done this way as Digi_tf locks it
                    if globalflags.DetDescrVersion.is_locked() :
                        globalflags.DetDescrVersion.unlock()
                    globalflags.DetDescrVersion.set_Value_and_Lock( metadatadict['SimLayout'] )
                    logDigitizationReadMetadata.warning("Set globalflags.DetDescrVersion = %s",globalflags.DetDescrVersion.get_Value())
        else:
            logDigitizationReadMetadata.error("SimLayout key not found in Simulation MetaData!")

    ## Check the Conditions Tag set against that used in the simulation
    if not skipCheck('IOVDbGlobalTag'):
        if 'IOVDbGlobalTag' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            if (digitizationFlags.IOVDbGlobalTag.statusOn):
                logDigitizationReadMetadata.info("Digitization properties: [IOVDbGlobalTag = %s], Signal Simulation MetaData: [IOVDbGlobalTag = %s]",
                                           digitizationFlags.IOVDbGlobalTag.get_Value(), metadatadict['IOVDbGlobalTag'])
            else:
                digitizationFlags.IOVDbGlobalTag = metadatadict['IOVDbGlobalTag']
                logDigitizationReadMetadata.debug("Set Digitization properties to match Signal Simulation Metadata: [IOVDbGlobalTag = %s]",
                                            digitizationFlags.IOVDbGlobalTag.get_Value())
        else:
            logDigitizationReadMetadata.error("IOVDbGlobalTag key not found in Simulation MetaData!")

    ## Set the TRTRangeCut digitizationFlag based on what was used during the simulation.
    if not skipCheck('TRTRangeCut'):
        if 'TRTRangeCut' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            if hasattr( digitizationFlags, 'TRTRangeCut'):
                digitizationFlags.TRTRangeCut = metadatadict['TRTRangeCut']
                logDigitizationReadMetadata.debug("Set Digitization properties to match Signal Simulation Metadata: [TRTRangeCut = %s]",
                                            digitizationFlags.TRTRangeCut.get_Value())
        else:
            logDigitizationReadMetadata.warning("TRTRangeCut key not found in Simulation MetaData!")

    ## Record the G4Version used in the simulation, so that Digitization Algorithms can use this information
    if not skipCheck('G4Version'):
        if 'G4Version' in simkeys:
            from Digitization.DigitizationFlags import digitizationFlags
            digitizationFlags.SimG4VersionUsed = metadatadict['G4Version']
            logDigitizationReadMetadata.debug("digitizationFlags.SimG4VersionUsed = Value from Sim Metadata = %s ", digitizationFlags.SimG4VersionUsed.get_Value())
        else:
            logDigitizationReadMetadata.error("G4Version key not found in Simulation MetaData!")

    ## Check which sub-detectors were simulated

    ## Digitization will only digitize detectors which have been simulated.
    ## If users want to digitize an un simulated detector this will be an expert
    ## action which will require hacking the python code.
    from AthenaCommon.DetFlags import DetFlags
    if not skipCheck('SimulatedDetectors'):
        if 'SimulatedDetectors' in simkeys:
            logDigitizationReadMetadata.debug("Switching off subdetectors which were not simulated")
            possibleSubDetectors=['pixel','SCT','TRT','BCM','Lucid','ZDC','ALFA','AFP','FwdRegion','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth']
            switchedOffSubDetectors=[]
            for subdet in possibleSubDetectors:
                if not subdet in metadatadict['SimulatedDetectors']:
                    attrname = subdet+"_setOff"
                    checkfn = getattr(DetFlags, attrname, None)
                    if checkfn is not None:
                        cmd='DetFlags.%s_setOff()' % subdet
                        logDigitizationReadMetadata.debug(cmd)
                        checkfn()
                    switchedOffSubDetectors+=[subdet]
            if switchedOffSubDetectors:
                logDigitizationReadMetadata.info("Ensured %s sub-detectors which were not simulated were switched off: %s", len(switchedOffSubDetectors), switchedOffSubDetectors)
            else:
                logDigitizationReadMetadata.info("All sub-detectors were simulated, so none needed to be switched off in digitization.")
            DetFlags.Print()

    ## Any other checks here
    logDigitizationReadMetadata.info("Completed checks of Digitization properties against Signal Simulation MetaData.")

def pileupMetaDataCheck(pileuptype, pileupfile, simdict):
    """Check the metadata for each pileup collection"""
    longpileuptype=expandPileUpType(pileuptype)
    result = True
    logDigitizationReadMetadata.info("Checking %s MetaData against Signal Simulation MetaData...", longpileuptype)
    import re
    metadatadict,result = buildDict(longpileuptype, pileupfile)
    pileupkeys = metadatadict.keys()
    simkeys = simdict.keys()
    if not result:
        logDigitizationReadMetadata.warning("Failed to Create %s Simulation MetaData Dictionary from file %s.", longpileuptype, pileupfile)
    else:
        ##Loop over MetaData keys which must have matching values
        SimKeysToCheck = [ 'PhysicsList', 'SimLayout', 'MagneticField','hitFileMagicNumber' ]#, 'WorldZRange' ]
        for o in SimKeysToCheck:
            if skipPileUpCheck(o, pileuptype):
                continue
            try:
                assert o in pileupkeys
            except AssertionError:
                logDigitizationReadMetadata.error("%s key missing from %s Simulation MetaData!", o, longpileuptype)
                raise AssertionError("Simulation MetaData key not found")
            try:
                assert o in simkeys
            except AssertionError:
                logDigitizationReadMetadata.error("%s key missing from Signal Simulation MetaData!", o)
                raise AssertionError("Simulation MetaData key not found")
            try:
                if not isinstance(metadatadict[o],type(simdict[o])):
                    assert re.match(str(metadatadict[o]), str(simdict[o]))
                else:
                    if isinstance(metadatadict[o],str):
                        assert re.match(metadatadict[o], simdict[o])
                    elif isinstance(metadatadict[o],int):
                        assert (metadatadict[o]==simdict[o])
                    else:
                        assert re.match(str(metadatadict[o]), str(simdict[o]))
            except AssertionError:
                logDigitizationReadMetadata.error("Simulation MetaData mismatch! %s: [%s = %s] Signal: [%s = %s]", longpileuptype, o, metadatadict[o], o, simdict[o])
                raise AssertionError("Simulation MetaData mismatch")
            logDigitizationReadMetadata.debug("%s Sim MetaData matches Signal Sim MetaData. [%s = %s]", longpileuptype, o, simdict[o])
        ##Ideally these keys would have matching values, but it should be OK if not.
        WarningKeys = [ 'IOVDbGlobalTag', 'G4Version' ]
        for o in WarningKeys:
            if skipPileUpCheck(o, pileuptype):
                continue
            try:
                assert o in pileupkeys
            except AssertionError:
                logDigitizationReadMetadata.error("%s key missing from %s Simulation MetaData!", o, longpileuptype)
                raise AssertionError("Simulation MetaData key not found")
            try:
                assert o in simkeys
            except AssertionError:
                logDigitizationReadMetadata.error("%s key missing from Signal Simulation MetaData!", o, longpileuptype)
                raise AssertionError("Simulation MetaData key not found")
            if not re.match(metadatadict[o], simdict[o]):
                logDigitizationReadMetadata.warning("Simulation MetaData mismatch! %s: [%s = %s] Signal: [%s = %s]", longpileuptype, o, metadatadict[o], o, simdict[o])
            else:
                logDigitizationReadMetadata.debug("%s Sim MetaData matches Signal Sim MetaData. [%s = %s]", longpileuptype, o, simdict[o])

        ## Check that the same sub-detectors were simulated in signal and background inputs
        if (not skipPileUpCheck('SimulatedDetectors', pileuptype)) and ('SimulatedDetectors' in simkeys):
            switchedOffSubDetectors=[]
            for subdet in simdict['SimulatedDetectors']:
                if not subdet in metadatadict['SimulatedDetectors']:
                    switchedOffSubDetectors+=[subdet]
            if switchedOffSubDetectors:
                logDigitizationReadMetadata.error("%s sub-detectors were sinmulated in the signal sample, but not in the %s background sample: %s", len(switchedOffSubDetectors), longpileuptype, switchedOffSubDetectors)
                raise AssertionError("Some simulated sub-detectors from signal sample are missing in the background samples.")
            else:
                logDigitizationReadMetadata.debug("All sub-detectors simulated in the signal sample were also simulated in the %s background sample.", longpileuptype)

    del metadatadict ## control where metadata can be used
    return result

def readHITSFileMetadata():
    logDigitizationReadMetadata.info("Checking for Signal Simulation MetaData...")
    import re
    import PyUtils.AthFile as af
    af.server.load_cache('digitization-afcache.ascii')

    #--------------------------------------------------
    # Check for the Run Number in the first Input file
    #--------------------------------------------------
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.dataRunNumber.statusOn :
        #if we're overriding the run number we need to keep track
        # of the HIT file run number - may as well read it here
        digitizationFlags.simRunNumber = int(digitizationFlags.getHitFileRunNumber(athenaCommonFlags.PoolHitsInput.get_Value()[0]))

    metadatadict,result = buildDict("Signal", athenaCommonFlags.PoolHitsInput.get_Value()[0])
    if result :
        signalMetaDataCheck(metadatadict)

        ## Check Pileup Simulation Parameters match those used for signal files
        from Digitization.DigitizationFlags import digitizationFlags
        pileuptype=""
        if digitizationFlags.doLowPtMinBias() and (not skipCheck('LowPtMinBias')):
            pileuptype="LowPtMinBias"
            if pileupMetaDataCheck(pileuptype, digitizationFlags.LowPtMinBiasInputCols.get_Value()[0],metadatadict):
                logDigitizationReadMetadata.info("Low Pt Minimum Bias Simulation MetaData matches Signal Simulation MetaData.")
        if digitizationFlags.doHighPtMinBias() and (not skipCheck('HighPtMinBias')):
            pileuptype="HighPtMinBias"
            if pileupMetaDataCheck(pileuptype, digitizationFlags.HighPtMinBiasInputCols.get_Value()[0],metadatadict):
                logDigitizationReadMetadata.info("High Pt Minimum Bias Simulation MetaData matches Signal Simulation MetaData.")
        if digitizationFlags.doCavern() and (not skipCheck('cavern')):
            pileuptype="cavern"
            ## For MC11 the cavern background will use a different physics
            ## list to the signal samples, so it is necessary to override
            ## the requirement that the PhysicsLists match.
            if not 'cavern_PhysicsList' in digitizationFlags.overrideMetadata.get_Value():
                digitizationFlags.overrideMetadata += ['cavern_PhysicsList']
            if pileupMetaDataCheck(pileuptype, digitizationFlags.cavernInputCols.get_Value()[0],metadatadict):
                logDigitizationReadMetadata.info("Cavern Background Simulation MetaData matches Signal Simulation MetaData.")
        if digitizationFlags.doBeamGas() and (not skipCheck('beamgas')):
            pileuptype="beamgas"
            if pileupMetaDataCheck(pileuptype, digitizationFlags.beamGasInputCols.get_Value()[0],metadatadict):
                logDigitizationReadMetadata.info("Beam Gas Simulation MetaData matches Signal Simulation MetaData.")
        if digitizationFlags.doBeamHalo() and (not skipCheck('beamhalo')):
            pileuptype="beamhalo"
            if pileupMetaDataCheck(pileuptype, digitizationFlags.beamHaloInputCols.get_Value()[0],metadatadict):
                logDigitizationReadMetadata.info("Beam Halo Simulation MetaData matches Signal Simulation MetaData.")
        ## All checks completed here
        logDigitizationReadMetadata.info("Completed all checks against Signal Simulation MetaData.")
    else:
        logDigitizationReadMetadata.info("Failed to Create Simulation MetaData Dictionary from file %s", athenaCommonFlags.PoolHitsInput.get_Value()[0])

    del metadatadict ## control where metadata can be used

    #raise SystemExit("Testing")
