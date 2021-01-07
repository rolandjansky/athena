# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

### This module contains functions which may need to peek at the input file metadata

from AthenaKernel.EventIdOverrideConfig import getMinMaxRunNumbers
## Get the logger
from AthenaCommon.Logging import logging
simMDlog = logging.getLogger('Sim_Metadata')

def fillAtlasMetadata(ConfigFlags, dbFiller):

    #add all ConfigFlags to the metadata
    #todo - only add certain ones?
    #in future this should be a ConfigFlags method...?
    for flag in sorted(ConfigFlags._flagdict): #only sim
        if "Sim" in flag:
            key = flag.split(".")[-1] #use final part of flag as the key
            value = eval("ConfigFlags."+flag)
            if not isinstance(value, str):
                value = str(value)
            dbFiller.addSimParam(key, value)
            simMDlog.info('SimulationMetaData: setting "%s" to be %s', key, value)

    dbFiller.addSimParam('G4Version', ConfigFlags.Sim.G4Version)
    dbFiller.addSimParam('RunType', 'atlas')
    dbFiller.addSimParam('beamType', ConfigFlags.Beam.Type)

    #---------  
    ## Simulated detector flags: add each enabled detector to the simulatedDetectors list
    simDets = []
    for det in ['Pixel','SCT','TRT','BCM','Lucid','ZDC','ALFA','AFP','FwdRegion','LAr','HGTD','Tile','MDT','CSC','TGC','RPC','MM','sTGC','Truth','LVL1']:
        attrname = "Detector.Geometry"+det
        if ConfigFlags.hasFlag(attrname):
            testValue = ConfigFlags._get(attrname)
            if testValue:
                simDets.append(det)
        else:
            simMDlog.info("No flag called '%s' found in ConfigFlags", attrname)

    simMDlog.info("Setting 'SimulatedDetectors' = %r", simDets)
    dbFiller.addSimParam('SimulatedDetectors', repr(simDets))

    ## Hard-coded simulation hit file magic number (for major changes)
    dbFiller.addSimParam('hitFileMagicNumber', '0') ##FIXME Remove this?


def fillISFMetadata(dbFiller):
    #todo - later
    from ISF_Config.ISF_jobProperties import ISF_Flags
    dbFiller.addSimParam('Simulator', ISF_Flags.Simulator())


def writeSimulationParametersMetadata(ConfigFlags):
    from IOVDbMetaDataTools import ParameterDbFiller
    dbFiller = ParameterDbFiller.ParameterDbFiller()
    myRunNumber, myEndRunNumber = getMinMaxRunNumbers(ConfigFlags)
    simMDlog.debug('ParameterDbFiller BeginRun = %s', str(myRunNumber) )
    dbFiller.setBeginRun(myRunNumber)
    simMDlog.debug('ParameterDbFiller EndRun   = %s', str(myEndRunNumber) )
    dbFiller.setEndRun(myEndRunNumber)

    fillAtlasMetadata(ConfigFlags, dbFiller)
    if ConfigFlags.Sim.ISFRun:
        #fillISFMetadata(dbFiller) #add later
        pass
    #-------------------------------------------------
    # Make the MetaData Db
    #-------------------------------------------------
    dbFiller.genSimDb()

    from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
    cfg = IOVDbSvcCfg(ConfigFlags)
    folder = "/Simulation/Parameters"
    dbConnection = "sqlite://;schema=SimParams.db;dbname=SIMPARAM"

    cfg.getService("IOVDbSvc").Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
    cfg.getService("IOVDbSvc").FoldersToMetaData += [ folder ]
    #cfg.getService("IOVSvc").partialPreLoadData = True #FIXME IOVSvc missing??
    return cfg
