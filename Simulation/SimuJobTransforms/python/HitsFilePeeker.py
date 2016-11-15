# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def hitColls2SimulatedDetectors(inputlist):
    """Build a dictionary from the list of containers in the metadata"""
    simulatedDetectors = []
    simulatedDictionary = {'PixelHits': 'pixel', 'SCT_Hits': 'SCT', 'TRTUncompressedHits': 'TRT',
                           'BCMHits': 'BCM', 'LucidSimHitsVector': 'Lucid', 'LArHitEMB': 'LAr',
                           'LArHitEMEC': 'LAr', 'LArHitFCAL': 'LAr', 'LArHitHEC': 'LAr', 'LArHitHGTD': 'HGTD',
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

def getHITSFile(runArgs):
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if hasattr(runArgs,"inputHITSFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputHITSFile )
        return runArgs.inputHITSFile[0]
    elif hasattr(runArgs,"inputHitsFile"):
        athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputHitsFile )
        return runArgs.inputHitsFile[0]
    else:
        raise SystemExit("No HITS file in runArgs!!")

def HitsFilePeeker(runArgs, skeletonLog):
    import PyUtils.AthFile as af
    try:
        f = af.fopen(getHITSFile(runArgs))
    except AssertionError:
        skeletonLog.error("Failed to open input file: %s", getHITSFile(runArgs))
    #check evt_type of input file
    if 'evt_type' in f.infos.keys():
        import re
        if not re.match(str(f.infos['evt_type'][0]), 'IS_SIMULATION') :
            skeletonLog.error('This input file has incorrect evt_type: %s',str(f.infos['evt_type']))
            skeletonLog.info('Please make sure you have set input file metadata correctly.')
            skeletonLog.info('Consider using the job transforms for earlier steps if you aren\'t already.')
            #then exit gracefully
            raise SystemExit("Input file evt_type is incorrect, please check your g4sim and evgen jobs.")
    else :
        skeletonLog.warning('Could not find \'evt_type\' key in athfile.infos. Unable to that check evt_type is correct.')
    metadatadict = dict()
    if 'metadata' in f.infos.keys():
        if '/Simulation/Parameters' in f.infos['metadata'].keys():
            metadatadict = f.infos['metadata']['/Simulation/Parameters']
            if isinstance(metadatadict, list):
                skeletonLog.warning("%s inputfile: %s contained %s sets of Simulation Metadata. Using the final set in the list.",inputtype,inputfile,len(metadatadict))
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
                    skeletonLog.warning("Failed to find IOVDbGlobalTag.")
    else:
        ##Patch for older hit files
        if 'SimulatedDetectors' not in metadatadict.keys():
            if 'eventdata_items' in f.infos.keys():
                metadatadict['SimulatedDetectors'] = hitColls2SimulatedDetectors(f.infos['eventdata_items'])
            else :
                metadatadict['SimulatedDetectors'] = ['pixel','SCT','TRT','BCM','Lucid','LAr','Tile','MDT','CSC','TGC','RPC','Truth']

    import re
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.DataSource="geant4"
    ## Configure DetDescrVersion
    if hasattr(runArgs,"geometryVersion"):
        inputGeometryVersion = runArgs.geometryVersion
        if type(inputGeometryVersion) == str and inputGeometryVersion.endswith("_VALIDATION"):
            inputGeometryVersion = inputGeometryVersion.replace("_VALIDATION", "")
        if 'SimLayout' in metadatadict.keys():
            if not re.match(metadatadict['SimLayout'], inputGeometryVersion):
                skeletonLog.warning("command-line geometryVersion (%s) does not match the value used in the Simulation step (%s) !",
                                    inputGeometryVersion, metadatadict['SimLayout'])
        globalflags.DetDescrVersion.set_Value_and_Lock( inputGeometryVersion )
        skeletonLog.info("Using geometryVersion from command-line: %s", globalflags.DetDescrVersion.get_Value())
    elif 'SimLayout' in metadatadict.keys():
        globalflags.DetDescrVersion.set_Value_and_Lock( metadatadict['SimLayout'] )
        skeletonLog.info("Using geometryVersion from HITS file metadata %s", globalflags.DetDescrVersion.get_Value())
    else:
        raise SystemExit("geometryVersion not found in HITS file metadata or on transform command-line!")

    ## Configure ConditionsTag
    if hasattr(runArgs,"conditionsTag"):
        if 'IOVDbGlobalTag' in metadatadict.keys():
            if not re.match(metadatadict['IOVDbGlobalTag'], runArgs.conditionsTag):
                skeletonLog.warning("command-line conditionsTag (%s) does not match the value used in the Simulation step (%s) !",
                                    runArgs.conditionsTag, metadatadict['IOVDbGlobalTag'])
            if not globalflags.ConditionsTag.is_locked():
                globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )
                skeletonLog.info("Using conditionsTag from command-line: %s", globalflags.ConditionsTag.get_Value())
            else:
                skeletonLog.info("globalflags.ConditionsTag already locked to %s - will not alter it.", globalflags.ConditionsTag.get_Value())
    elif 'IOVDbGlobalTag' in metadatadict.keys():
        globalflags.ConditionsTag.set_Value_and_Lock( metadatadict['IOVDbGlobalTag'] )
        skeletonLog.info("Using conditionsTag from HITS file metadata %s", globalflags.ConditionsTag.get_Value())
    else:
        skeletonLog.fatal("conditionsTag not found in HITS file metadata or on transform command-line!")
        raise SystemExit("conditionsTag not found in HITS file metadata or on transform command-line!")

    ## Configure DetFlags
    if 'SimulatedDetectors' in metadatadict.keys():
        from AthenaCommon.DetFlags import DetFlags
        # by default everything is off
        DetFlags.all_setOff()
        skeletonLog.debug("Switching on DetFlags for subdetectors which were simulated")
        for subdet in metadatadict['SimulatedDetectors']:
            cmd='DetFlags.%s_setOn()' % subdet
            skeletonLog.debug(cmd)
            try:
                exec cmd
            except:
                skeletonLog.warning('Failed to switch on subdetector %s',subdet)
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.digitize.all_setOff()
        if hasattr(DetFlags, 'overlay'): DetFlags.overlay.all_setOff()
        DetFlags.pileup.all_setOff()
        DetFlags.readRDOBS.all_setOff()
        DetFlags.readRDOPool.all_setOff()
        DetFlags.readRIOBS.all_setOff()
        DetFlags.readRIOPool.all_setOff()
        DetFlags.makeRIO.all_setOff()
        DetFlags.writeBS.all_setOff()
        DetFlags.writeRDOPool.all_setOff()
        DetFlags.writeRIOPool.all_setOff()

    return
