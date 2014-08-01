# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def RDOFilePeeker(runArgs, skeletonLog):
    import PyUtils.AthFile as af
    try:
        f = af.fopen(runArgs.inputRDOFile[0])
    except AssertionError:
        skeletonLog.error("Failed to open input file: %s", runArgs.inputRDOFile[0])
    #check evt_type of input file
    if 'evt_type' in f.infos.keys():
        import re
        if not re.match(str(f.infos['evt_type'][0]), 'IS_SIMULATION') :
            skeletonLog.error('This input file has incorrect evt_type: %s',str(f.infos['evt_type']))
            skeletonLog.info('Please make sure you have set input file metadata correctly.')
            skeletonLog.info('Consider using the job transforms for earlier steps if you aren\'t already.')
            #then exit gracefully
            raise SystemExit("Input file evt_type is incorrect, please check your digi, g4sim and evgen jobs.")
    else :
        skeletonLog.warning('Could not find \'evt_type\' key in athfile.infos. Unable to that check evt_type is correct.')
    metadatadict = dict()
    if 'metadata' in f.infos.keys():
        if '/Digitization/Parameters' in f.infos['metadata'].keys():
            metadatadict = f.infos['metadata']['/Digitization/Parameters']
            if isinstance(metadatadict, list):
                skeletonLog.warning("%s inputfile: %s contained %s sets of Dititization Metadata. Using the final set in the list.",inputtype,inputfile,len(metadatadict))
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
        if 'DigitizedDetectors' not in metadatadict.keys():
            metadatadict['DigitizedDetectors'] = ['pixel','SCT','TRT','BCM','Lucid','LAr','Tile','MDT','CSC','TGC','RPC','Truth']

    import re
    from AthenaCommon.GlobalFlags import globalflags

    ## Configure DetDescrVersion
    if hasattr(runArgs,"geometryVersion"):
        inputGeometryVersion = runArgs.geometryVersion
        if type(inputGeometryVersion) == str and inputGeometryVersion.endswith("_VALIDATION"):
            inputGeometryVersion = inputGeometryVersion.replace("_VALIDATION", "")
        if 'DetDescrVersion' in metadatadict.keys():
            if not re.match(metadatadict['DetDescrVersion'], inputGeometryVersion):
                skeletonLog.warning("command-line geometryVersion (%s) does not match the value used in the Simulation step (%s) !",
                                    inputGeometryVersion, metadatadict['DetDescrVersion'])
        globalflags.DetDescrVersion.set_Value_and_Lock( inputGeometryVersion )
        skeletonLog.info("Using geometryVersion from command-line: %s", globalflags.DetDescrVersion.get_Value())
    elif 'DetDescrVersion' in metadatadict.keys():
        globalflags.DetDescrVersion.set_Value_and_Lock( metadatadict['DetDescrVersion'] )
        skeletonLog.info("Using geometryVersion from RDO file metadata %s", globalflags.DetDescrVersion.get_Value())
    else:
        raise SystemExit("geometryVersion not found in RDO file metadata or on transform command-line!")

    ## Configure ConditionsTag
    if hasattr(runArgs,"conditionsTag"):
        if 'IOVDbGlobalTag' in metadatadict.keys():
            if not re.match(metadatadict['IOVDbGlobalTag'], runArgs.conditionsTag):
                skeletonLog.warning("command-line conditionsTag (%s) does not match the value used in the Simulation step (%s) !",
                                    runArgs.conditionsTag, metadatadict['IOVDbGlobalTag'])
        #globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag ) ## already done in CommonSkeletonJobOptions.py
        skeletonLog.info("Using conditionsTag from command-line: %s", globalflags.ConditionsTag.get_Value())
    elif 'IOVDbGlobalTag' in metadatadict.keys():
        globalflags.ConditionsTag.set_Value_and_Lock( metadatadict['IOVDbGlobalTag'] )
        skeletonLog.info("Using conditionsTag from RDO file metadata %s", globalflags.ConditionsTag.get_Value())
    else:
        raise SystemExit("conditionsTag not found in RDO file metadata or on transform command-line!")

    ## Configure DetFlags
    if 'DigitizedDetectors' in metadatadict.keys():
        from AthenaCommon.DetFlags import DetFlags
        # by default everything is off
        DetFlags.all_setOff()
        skeletonLog.debug("Switching on DetFlags for subdetectors which were simulated")
        for subdet in metadatadict['DigitizedDetectors']:
            cmd='DetFlags.%s_setOn()' % subdet
            skeletonLog.debug(cmd)
            try:
                exec cmd
            except:
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

