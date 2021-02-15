from past.builtins import basestring

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def RDOFilePeeker(runArgs, skeletonLog):
    from PyUtils.MetaReader import read_metadata
    try:
        input_file = runArgs.inputRDOFile[0]
        metadata_lite = read_metadata(input_file)  # Use this only to read the key 'eventTypes', which is promoted in 'lite' mode.
        # promote keys stored under input filename key one level up to access them directly
        metadata_lite = metadata_lite[input_file]
        # use the mode 'full' to access all metadata (needed for '/Digitization/Parameters')
        metadata = read_metadata(input_file, mode= 'full')
        # promote keys stored under input filename key one level up to access them directly
        metadata = metadata[input_file]
    except AssertionError:
        skeletonLog.error("Failed to open input file: %s", runArgs.inputRDOFile[0])
    #check eventTypes of input file
    if 'eventTypes' in metadata_lite:
        if 'IS_SIMULATION' not in metadata_lite['eventTypes']:
            skeletonLog.error('This input file has incorrect eventTypes: %s', metadata_lite['eventTypes'])
            skeletonLog.info('Please make sure you have set input file metadata correctly.')
            skeletonLog.info('Consider using the job transforms for earlier steps if you aren\'t already.')
            #then exit gracefully
            raise SystemExit("Input file eventTypes is incorrect, please check your digi, g4sim and evgen jobs.")
    else:
        skeletonLog.warning('Could not find \'eventTypes\' key in MetaReader -> metadata. Unable to that check if eventTypes is correct.')

    metadatadict = {}
    if '/Digitization/Parameters' in metadata:
        metadatadict = metadata['/Digitization/Parameters']
        if isinstance(metadatadict, list):
            skeletonLog.warning("inputfile: %s contained %s sets of Dititization Metadata. Using the final set in the list.",input_file,len(metadatadict))
            metadatadict = metadatadict[-1]
    ##Get IOVDbGlobalTag
        if 'IOVDbGlobalTag' not in metadatadict:
            try:
                if metadata['/TagInfo']['IOVDbGlobalTag'] is not None:
                    metadatadict['IOVDbGlobalTag'] = metadata['/TagInfo']['IOVDbGlobalTag']
            except Exception:
                skeletonLog.warning("Failed to find IOVDbGlobalTag.")
    else:
        ##Patch for older hit files
        if 'DigitizedDetectors' not in metadatadict:
            metadatadict['DigitizedDetectors'] = ['pixel','SCT','TRT','BCM','Lucid','LAr','Tile','MDT','CSC','TGC','RPC','Truth']

    import re
    from AthenaCommon.GlobalFlags import globalflags

    ## Configure DetDescrVersion
    if hasattr(runArgs,"geometryVersion"):
        inputGeometryVersion = runArgs.geometryVersion
        if isinstance(inputGeometryVersion, basestring) and inputGeometryVersion.endswith("_VALIDATION"):
            inputGeometryVersion = inputGeometryVersion.replace("_VALIDATION", "")
        if 'DetDescrVersion' in metadatadict:
            if not re.match(metadatadict['DetDescrVersion'], inputGeometryVersion):
                skeletonLog.warning("command-line geometryVersion (%s) does not match the value used in the Simulation step (%s) !",
                                    inputGeometryVersion, metadatadict['DetDescrVersion'])
        globalflags.DetDescrVersion.set_Value_and_Lock( inputGeometryVersion )
        skeletonLog.info("Using geometryVersion from command-line: %s", globalflags.DetDescrVersion.get_Value())
    elif 'DetDescrVersion' in metadatadict:
        globalflags.DetDescrVersion.set_Value_and_Lock( metadatadict['DetDescrVersion'] )
        skeletonLog.info("Using geometryVersion from RDO file metadata %s", globalflags.DetDescrVersion.get_Value())
    else:
        raise SystemExit("geometryVersion not found in RDO file metadata or on transform command-line!")

    ## Configure ConditionsTag
    if hasattr(runArgs,"conditionsTag"):
        if 'IOVDbGlobalTag' in metadatadict:
            if not re.match(metadatadict['IOVDbGlobalTag'], runArgs.conditionsTag):
                skeletonLog.warning("command-line conditionsTag (%s) does not match the value used in the Simulation step (%s) !",
                                    runArgs.conditionsTag, metadatadict['IOVDbGlobalTag'])
        #globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag ) ## already done in CommonSkeletonJobOptions.py
        skeletonLog.info("Using conditionsTag from command-line: %s", globalflags.ConditionsTag.get_Value())
    elif 'IOVDbGlobalTag' in metadatadict:
        globalflags.ConditionsTag.set_Value_and_Lock( metadatadict['IOVDbGlobalTag'] )
        skeletonLog.info("Using conditionsTag from RDO file metadata %s", globalflags.ConditionsTag.get_Value())
    else:
        raise SystemExit("conditionsTag not found in RDO file metadata or on transform command-line!")

    ## Configure DetFlags
    if 'DigitizedDetectors' in metadatadict:
        from AthenaCommon.DetFlags import DetFlags
        # by default everything is off
        DetFlags.all_setOff()
        skeletonLog.debug("Switching on DetFlags for subdetectors which were simulated")
        for subdet in metadatadict['DigitizedDetectors']:
            cmd='DetFlags.%s_setOn()' % subdet
            skeletonLog.debug(cmd)
            try:
                exec(cmd)
            except Exception:
                skeletonLog.warning('Failed to switch on subdetector %s',subdet)
        #hacks to reproduce the sub-set of DetFlags left on by RecExCond/AllDet_detDescr.py
        DetFlags.simulate.all_setOff()
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.digitize.all_setOff()
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

