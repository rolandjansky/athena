from past.builtins import basestring

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
        if entry[1] in simulatedDictionary:
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
    peekInfo = dict()
    peekInfo["AntiKt4TruthJetsPresent"] = False
    peekInfo["AntiKt6TruthJetsPresent"] = False
    peekInfo["PileUpTruthParticlesPresent"] = False
    from PyUtils.MetaReader import read_metadata
    try:
        input_file = getHITSFile(runArgs)
        metadata_peeker = read_metadata(input_file, mode = 'peeker')
        metadata_peeker = metadata_peeker[input_file]  # promote all keys one level up

        metadata_full = read_metadata(input_file, mode = 'full')
        metadata_full = metadata_full[input_file]  # promote all keys one level up
    except AssertionError:
        skeletonLog.error("Failed to open input file: %s", getHITSFile(runArgs))
    # check eventTypes of input file
    if 'eventTypes' in metadata_peeker:
        import re
        if 'IS_SIMULATION' not in metadata_peeker['eventTypes']:
            skeletonLog.error('This input file has incorrect evt_type: %s', metadata_peeker['eventTypes'])
            skeletonLog.info('Please make sure you have set input file metadata correctly.')
            skeletonLog.info('Consider using the job transforms for earlier steps if you aren\'t already.')
            #then exit gracefully
            raise SystemExit("Input file eventTypes is incorrect, please check your g4sim and evgen jobs.")
    else :
        skeletonLog.warning('Could not find \'eventTypes\' key in metadata. Unable to that check eventTypes is correct.')

    metadatadict = dict()

    if metadata_full:
        if '/Simulation/Parameters' in metadata_full:
            metadatadict = metadata_full['/Simulation/Parameters']

        ##Get IOVDbGlobalTag
        if 'IOVDbGlobalTag' not in metadatadict:
            try:
                assert metadata_full['/TagInfo']['IOVDbGlobalTag'] is not None
                metadatadict['IOVDbGlobalTag'] = metadata_full['/TagInfo']['IOVDbGlobalTag']
            except Exception:
                try:
                    assert metadata_full['/Digitization/Parameters']['IOVDbGlobalTag'] is not None
                    metadatadict['IOVDbGlobalTag'] = metadata_full['/Digitization/Parameters']['IOVDbGlobalTag']
                except Exception:
                    skeletonLog.warning("Failed to find IOVDbGlobalTag.")
    else:
        ##Patch for older hit files
        if 'SimulatedDetectors' not in metadatadict:
            if 'itemList' in metadata_peeker:
                metadatadict['SimulatedDetectors'] = hitColls2SimulatedDetectors(metadata_peeker['itemList'])
            else :
                metadatadict['SimulatedDetectors'] = ['pixel','SCT','TRT','BCM','Lucid','LAr','Tile','MDT','CSC','TGC','RPC','Truth']

    # Check for Truth Containers
    if 'itemList' in metadata_peeker:
        for entry in metadata_peeker['itemList']:
            if 'AntiKt4TruthJets' == entry[1]:
                peekInfo["AntiKt4TruthJetsPresent"] = True
            if 'AntiKt6TruthJets' == entry[1]:
                peekInfo["AntiKt6TruthJetsPresent"] = True
            if 'TruthPileupParticles' == entry[1]:
                peekInfo["PileUpTruthParticlesPresent"] = True

    from AthenaCommon.GlobalFlags import globalflags
    globalflags.DataSource="geant4"
    ## Configure DetDescrVersion
    if hasattr(runArgs,"geometryVersion"):
        inputGeometryVersion = runArgs.geometryVersion
        if isinstance(inputGeometryVersion, basestring) and inputGeometryVersion.endswith("_VALIDATION"):
            inputGeometryVersion = inputGeometryVersion.replace("_VALIDATION", "")
        if 'SimLayout' in metadatadict:
            if not re.match(metadatadict['SimLayout'], inputGeometryVersion):
                skeletonLog.warning("command-line geometryVersion (%s) does not match the value used in the Simulation step (%s) !",
                                    inputGeometryVersion, metadatadict['SimLayout'])
        globalflags.DetDescrVersion.set_Value_and_Lock( inputGeometryVersion )
        skeletonLog.info("Using geometryVersion from command-line: %s", globalflags.DetDescrVersion.get_Value())
    elif 'SimLayout' in metadatadict:
        globalflags.DetDescrVersion.set_Value_and_Lock( metadatadict['SimLayout'] )
        skeletonLog.info("Using geometryVersion from HITS file metadata %s", globalflags.DetDescrVersion.get_Value())
    else:
        raise SystemExit("geometryVersion not found in HITS file metadata or on transform command-line!")

    ## Configure ConditionsTag
    if hasattr(runArgs,"conditionsTag"):
        if 'IOVDbGlobalTag' in metadatadict:
            if not re.match(metadatadict['IOVDbGlobalTag'], runArgs.conditionsTag):
                skeletonLog.warning("command-line conditionsTag (%s) does not match the value used in the Simulation step (%s) !",
                                    runArgs.conditionsTag, metadatadict['IOVDbGlobalTag'])
            if not globalflags.ConditionsTag.is_locked():
                globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )
                skeletonLog.info("Using conditionsTag from command-line: %s", globalflags.ConditionsTag.get_Value())
            else:
                skeletonLog.info("globalflags.ConditionsTag already locked to %s - will not alter it.", globalflags.ConditionsTag.get_Value())
    elif 'IOVDbGlobalTag' in metadatadict:
        globalflags.ConditionsTag.set_Value_and_Lock( metadatadict['IOVDbGlobalTag'] )
        skeletonLog.info("Using conditionsTag from HITS file metadata %s", globalflags.ConditionsTag.get_Value())
    else:
        skeletonLog.fatal("conditionsTag not found in HITS file metadata or on transform command-line!")
        raise SystemExit("conditionsTag not found in HITS file metadata or on transform command-line!")

    ## Configure DetFlags
    if 'SimulatedDetectors' in metadatadict:
        from AthenaCommon.DetFlags import DetFlags
        # by default everything is off
        DetFlags.all_setOff()
        skeletonLog.debug("Switching on DetFlags for subdetectors which were simulated")
        simulatedDetectors = eval(metadatadict['SimulatedDetectors'])
        for subdet in simulatedDetectors:
            cmd='DetFlags.%s_setOn()' % subdet
            skeletonLog.debug(cmd)
            try:
                exec(cmd)
            except Exception:
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
        print('{} -> __Test__001__:\n{}'.format(__file__, metadatadict))
    return peekInfo
