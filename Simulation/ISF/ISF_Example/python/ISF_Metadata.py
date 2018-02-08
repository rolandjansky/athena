# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### This module contains functions which may need to peek at the input file metadata

## Get the logger
from AthenaCommon.Logging import *
simMDlog = logging.getLogger('Sim_Metadata')

def getAthFile():
    ## Allow the input check to be skipped.  This should only be done in production
    ##  jobs, in order to avoid peeking and spoiling performance on some systems
    inputAthFileObject = None
    import os
    if not ('G4ATLAS_SKIPFILEPEEK' in os.environ and os.environ['G4ATLAS_SKIPFILEPEEK']):
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.PoolEvgenInput.statusOn:
            try:
                import PyUtils.AthFile as af
                inputAthFileObject = af.fopen(athenaCommonFlags.PoolEvgenInput()[0])
            except:
                simMDlog.warning("AthFile failed to open %s", athenaCommonFlags.PoolEvgenInput()[0])
    else:
        simMDlog.info("G4ATLAS_SKIPFILEPEEK environment variable present, so skipping all input file peeking.")
    return inputAthFileObject

inputAthFileObject = getAthFile()


### Check that we aren't trying to pass a data file as the input to the simulation!
def inputFileValidityCheck():
    from ISF_Example.ISF_Metadata import inputAthFileObject
    if inputAthFileObject is not None:
        ## Check that event type is SIMULATION (as it must be)
        if "evt_type" in inputAthFileObject.infos.keys():
            evttypes = inputAthFileObject.infos["evt_type"]
            evttype0 = str(evttypes[0])
            if not evttype0.startswith("IS_SIMULATION"):
                msg =  "This input file has incorrect evt_type: %s\n" % str(evttypes)
                msg += "Please make sure you have set input file metadata correctly - "
                msg += "consider using the job transforms for earlier steps if you aren't already doing so."
                simMDlog.fatal(msg)
                raise SystemExit("Input file evt_type is incorrect: please check your evgen jobs.")
        else:
            simMDlog.warning("Could not find 'evt_type' key in athfile.infos. Unable to that check evt_type is correct.")
    else:
        simMDlog.info("No input Evgen AthFile object available, so skipping check for input file validity.")

### Read in special simulation job option fragments based on metadata passed by the evgen stage
def checkForSpecialConfigurationMetadata():
    from ISF_Example.ISF_Metadata import inputAthFileObject
    if inputAthFileObject is not None:
        if "specialConfiguration" in inputAthFileObject.infos["tag_info"]:
            from G4AtlasApps.SimFlags import simFlags
            simFlags.specialConfiguration = dict()
            item = inputAthFileObject.infos["tag_info"]["specialConfiguration"]
            ## Parse the specialConfiguration string
            ## Format is 'key1=value1;key2=value2;...'. or just '
            spcitems = item.split(";")
            params = {}
            someIncludes=[]
            for spcitem in spcitems:
                #print spcitem
                ## Ignore empty or "NONE" substrings, e.g. from consecutive or trailing semicolons
                if not spcitem or spcitem.upper() == "NONE":
                    continue
                ## If not in key=value format, treat as v, with k="preInclude"
                if "=" not in spcitem:
                    spcitem = "preInclude=" + spcitem
                ## Handle k=v directives
                k, v = spcitem.split("=")
                if k == "preInclude":
                    someIncludes+=[v]
                else:
                    params[k] = v
            simMDlog.info(params)
            simFlags.specialConfiguration.get_Value().update(params)
            from AthenaCommon.Include import include
            for inc in someIncludes:
                include(inc)
    else:
        simMDlog.info("No input Evgen AthFile object available so skipping check for specialConfiguration metadata.")

def fillCommonMetadata(dbFiller):
    pass

def fillAtlasMetadata(dbFiller):
    ## Write sim parameters as metadata keys
    from AthenaCommon.JobProperties import JobProperty
    from G4AtlasApps.SimFlags import simFlags
    simParams = [sf for sf in dir(simFlags) if isinstance(getattr(simFlags, sf), JobProperty)]
    for sp in simParams:
        ## Don't write out random number seeds or RunDict as metadata
        if sp in ("RandomSeedList", "RandomSeedOffset", "RunDict"):
            continue
        ## Don't write out Tool and Service names
        if sp in ("TruthService"):
            continue
        ## Only store InitFunction names
        if sp in ("InitFunctions"):
            initfuncdict = dict()
            for level in getattr(simFlags, sp).get_Value():
                initfuncdict.setdefault(level, [])
                for element in getattr(simFlags, sp).get_Value()[level]:
                    initfuncdict[level].append(element.__name__)
            testValue = str( initfuncdict ) if getattr(simFlags, sp).statusOn else 'default'
            dbFiller.addSimParam(sp, testValue)
            continue
        testValue = str( getattr(simFlags, sp).get_Value() ) if getattr(simFlags, sp).statusOn else 'default'
        dbFiller.addSimParam(sp, testValue)
    import os
    dbFiller.addSimParam('G4Version', str(os.environ['G4VERS']))
    dbFiller.addSimParam('RunType', 'atlas')
    from AthenaCommon.BeamFlags import jobproperties
    dbFiller.addSimParam('beamType', jobproperties.Beam.beamType.get_Value())

    ## Simulated detector flags: add each enabled detector to the simulatedDetectors list
    from AthenaCommon.DetFlags import DetFlags
    simDets = []
    for det in ['pixel','SCT','TRT','BCM','DBM','Lucid','FwdRegion','ZDC','ALFA','AFP','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth']:
        attrname = det+"_on"
        checkfn = getattr(DetFlags.geometry, attrname, None)
        if checkfn is None:
            continue
        if checkfn():
            simDets.append(det)
    dbFiller.addSimParam('SimulatedDetectors', repr(simDets))

    ## Hard-coded simulation hit file magic number (for major changes)
    dbFiller.addSimParam('hitFileMagicNumber', '0') ##FIXME Remove this?

def fillTestBeamMetadata(dbFiller):
    dbFiller.addSimParam('RunType','testbeam')
    SimParams = ['CalibrationRun','DoLArBirk','LArParameterization',
                 'MagneticField','PhysicsList','Seeds','SeedsG4','SimLayout',
                 'WorldZRange','NeutronTimeCut','NeutronEnergyCut','ApplyEMCuts','RunNumber']
    for o in [ o for o in SimParams if simFlags.__dict__.keys().__contains__(o) ]:
        testValue = 'default'
        if (simFlags.__dict__.__contains__(o) and simFlags.__dict__.get(o).statusOn) :
            testValue = simFlags.__dict__.get(o).get_Value()
            if not isinstance(testValue, str):
                testValue = str(simFlags.__dict__.get(o).get_Value())
        dbFiller.addSimParam(o,  testValue)
    if (simFlags.__dict__.__contains__('EventFilter') and simFlags.EventFilter.statusOn) :
        dbFiller.addSimParam('EtaPhiStatus', str(simFlags.EventFilter.get_Value()['EtaPhiFilters']))
        dbFiller.addSimParam('VRangeStatus', str(simFlags.EventFilter.get_Value()['VertexRangeChecker']))
    else :
        dbFiller.addSimParam('EtaPhiStatus', 'default')
        dbFiller.addSimParam('VRangeStatus', 'default')
    dbFiller.addSimParam('G4Version', str(os.environ['G4VERS']))
    dbFiller.addSimParam('beamType',jobproperties.Beam.beamType.get_Value())

    ####### Hard coded simulation hit file magic number (for major changes) ######
    dbFiller.addSimParam('hitFileMagicNumber','0')

def fillISFMetadata(dbFiller):
    from ISF_Config.ISF_jobProperties import ISF_Flags
    dbFiller.addSimParam('Simulator', ISF_Flags.Simulator())

def createSimulationParametersMetadata():
    from IOVDbMetaDataTools import ParameterDbFiller
    dbFiller = ParameterDbFiller.ParameterDbFiller()
    from ISF_Example.ISF_Metadata import inputAthFileObject
    ## Set run numbers
    minrunnum = 0
    maxrunnum = 2147483647 # MAX
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags, 'RunNumber') and simFlags.RunNumber.statusOn:
       minrunnum = simFlags.RunNumber()
       ## FIXME need to use maxrunnum = 2147483647 for now to keep overlay working but in the future this should be set properly.
       #maxrunnum = minrunnum + 1
    elif inputAthFileObject is not None:
       if len(inputAthFileObject.run_numbers) > 0:
           minrunnum = inputAthFileObject.run_numbers[0]
           maxrunnum = minrunnum + 1
       else:
           raise Exception('IllegalRunNumber')
    else:
        simMDlog.info('Skipping run number setting - would need to set simFlags.RunNumber for this.')
    simMDlog.info("Using the following run number range for MetaData IOV: ("+str(minrunnum)+","+str(maxrunnum)+").")
    dbFiller.setBeginRun(minrunnum)
    dbFiller.setEndRun(maxrunnum)

    fillAtlasMetadata(dbFiller)
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

def createTBSimulationParametersMetadata():
    from IOVDbMetaDataTools import ParameterDbFiller
    dbFiller = ParameterDbFiller.ParameterDbFiller()
    from ISF_Example.ISF_Metadata import inputAthFileObject
    ## Set run numbers
    minrunnum = 0
    maxrunnum = 2147483647 # MAX
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags, 'RunNumber') and simFlags.RunNumber.statusOn:
       minrunnum = simFlags.RunNumber()
       ## FIXME need to use maxrunnum = 2147483647 for now to keep overlay working but in the future this should be set properly.
       #maxrunnum = minrunnum + 1
    elif inputAthFileObject is not None:
       if len(inputAthFileObject.run_numbers) > 0:
           minrunnum = inputAthFileObject.run_numbers[0]
           maxrunnum = minrunnum + 1
       else:
           raise Exception('IllegalRunNumber')
    else:
        simMDlog.info('Skipping run number setting - would need to set simFlags.RunNumber for this.')
    simMDlog.info("Using the following run number range for MetaData IOV: ("+str(minrunnum)+","+str(maxrunnum)+").")
    dbFiller.setBeginRun(myMinRunNumber)
    dbFiller.setEndRun(myMaxRunNumber)

    fillTestBeamMetadata(dbFiller)
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

# Check on run numbers and update them if necessary
# copied from do_run_number_modifications() in PyG4Atlas.py
def configureRunNumberOverrides():
    myRunNumber = 1
    myFirstLB = 1
    myInitialTimeStamp = 0
    from ISF_Example.ISF_Metadata import inputAthFileObject
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags, "RunNumber") and simFlags.RunNumber.statusOn:
        myRunNumber = simFlags.RunNumber.get_Value()
        simMDlog.info('Found run number %d in sim flags.' % myRunNumber)
        ## Set event selector details based on evgen metadata

        ######update the run/event info for each event
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr,'EvtIdModifierSvc'):
            import AthenaServices.Configurables as asc
            ServiceMgr +=asc.EvtIdModifierSvc(EvtStoreName="StoreGateSvc")
            from AthenaCommon.AppMgr import theApp
            theApp.CreateSvc += ["EvtIdModifierSvc"]
        else:
             simMDlog.warning('Will override the settings of the EvtIdModifierSvc that was previously set up!')
        #fix iov metadata
        if not hasattr(ServiceMgr.ToolSvc, 'IOVDbMetaDataTool'):
            from AthenaCommon import CfgMgr
            ServiceMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
        ServiceMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = [myRunNumber, 2147483647]
        ## FIXME need to use maxRunNumber = 2147483647 for now to keep overlay working but in the future this should be set properly.
        # Using event numbers to avoid "some very large number" setting
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        totalNumber = 1000000 # TODO possibly get this from AthFile??
        if athenaCommonFlags.EvtMax() is not None and athenaCommonFlags.EvtMax()>0: totalNumber = athenaCommonFlags.EvtMax()+1
        if athenaCommonFlags.SkipEvents() is not None and athenaCommonFlags.SkipEvents()>0: totalNumber += athenaCommonFlags.SkipEvents()
        try:
          from RunDependentSimComps.RunDMCFlags import runDMCFlags
          myInitialTimeStamp = runDMCFlags.RunToTimestampDict.getTimestampForRun(myRunNumber)
          #print "FOUND TIMESTAMP ", str(myInitialTimeStamp)
        except:
          myInitialTimeStamp = 1
        ServiceMgr.EvtIdModifierSvc.add_modifier(run_nbr=myRunNumber, lbk_nbr=myFirstLB, time_stamp=myInitialTimeStamp, nevts=totalNumber)
        if hasattr(ServiceMgr.EventSelector,'OverrideRunNumberFromInput'): ServiceMgr.EventSelector.OverrideRunNumberFromInput = True
    elif inputAthFileObject is not None:
        ## Get evgen run number and lumi block
        if len(inputAthFileObject.run_numbers) > 0:
            myRunNumber = inputAthFileObject.run_numbers[0]
            simMDlog.info('Found run number %d in hits file metadata.'% myRunNumber)
        else:
            simMDlog.warning('Failed to find run number in hits file metadata.')
        if inputAthFileObject.lumi_block:
            myFirstLB = inputAthFileObject.lumi_block[0]
    else:
        simMDlog.warning('Requires simFlags.RunNumber to be specified in this running mode.\
            Using default value of 1 for RunNumber.')

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.EventSelector.OverrideRunNumber = True
    ServiceMgr.EventSelector.RunNumber = myRunNumber
    ServiceMgr.EventSelector.FirstLB = myFirstLB
    # Necessary to avoid a crash
    ServiceMgr.EventSelector.InitialTimeStamp = myInitialTimeStamp
