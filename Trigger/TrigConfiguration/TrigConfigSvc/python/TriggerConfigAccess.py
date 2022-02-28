# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from .TrigConfigSvcCfg import getTrigConfigFromFlag, getL1MenuFileName, getHLTMenuFileName, getL1PrescalesSetFileName, getHLTPrescalesSetFileName, getBunchGroupSetFileName, getHLTJobOptionsFileName, getHLTMonitoringFileName

from TrigConfIO.L1TriggerConfigAccess import L1MenuAccess, L1PrescalesSetAccess, BunchGroupSetAccess
from TrigConfIO.HLTTriggerConfigAccess import HLTMenuAccess, HLTPrescalesSetAccess, HLTJobOptionsAccess, HLTMonitoringAccess

from functools import lru_cache

"""
Access to the trigger configuration in python is provided depending on
the trigger configuration source

The tc source is taken from the TriggerFlag triggerConfig

1) tc source is set to INFILE

This is usually the case when running on ESD, AOD, and dAOD files and
only in this case. An exception is RDO with trigger information in
MC. The menu and prescales are taken from the pool file, from the
in-file metadata.


2) tc source is set to FILE

This is the case when executing the trigger with the configuration
taken from file.  The filename is provided by the function
@getL1MenuFileName

3) tc source is set to DB

This is the case when executing the trigger from the DB. The DB
connection and keys are provided by the triggerConfig flag

4) tc source is COOL

This is the case when reconstructing the data. From COOL the
configuration keys and db alias are taken, the configurations
are then loaded from the DB.

"""

@lru_cache(maxsize=None)
def getKeysFromCool(runNr, lbNr = 0):
    """Return dictionary of trigger keys for given run and lumiblock number
    """
    from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil
    condb = "CONDBR2" if runNr > 236108 else "COMP200"
    db = TriggerCoolUtil.GetConnection(condb)
    run_range = [[runNr,runNr]]
    d = { k: TriggerCoolUtil.getHLTConfigKeys(db, run_range)[runNr][k] for k in ['SMK', 'DB'] }
    for ( key, lbfirst, lblast) in TriggerCoolUtil.getBunchGroupKey(db, run_range)[runNr]['BGKey']:
        if lbNr>=lbfirst and (lbNr<=lblast or lblast==-1):
            d['BGSK'] = key
            break
    for ( key, lbfirst, lblast) in TriggerCoolUtil.getL1ConfigKeys(db, run_range)[runNr]['LVL1PSK']:
        if lbNr>=lbfirst and (lbNr<=lblast or lblast==-1):
            d['L1PSK'] = key
            break
    for ( key, lbfirst, lblast) in TriggerCoolUtil.getHLTPrescaleKeys(db, run_range)[runNr]['HLTPSK2']:
        if lbNr>=lbfirst and (lbNr<=lblast or lblast==-1):
            d['HLTPSK'] = key
            break

    # dbalias mapping
    dbaliasMapping = { "TRIGGERDBR2R" : "TRIGGERDB",
                       "TRIGGERDBV2" : "TRIGGERDB_RUN1" }
    if d["DB"] in dbaliasMapping:
        d["DB"] = dbaliasMapping[ d["DB"] ]

    return d

"""
Returns a string-serialised JSON object from the metadata store.
Checks AOD syntax first, then fully-qualified ESD syntax
"""
def _getJSONFromMetadata(flags, key):
    from AthenaConfiguration.Enums import Format
    if flags.Input.Format != Format.POOL:
        raise RuntimeError("Cannot read trigger configuration (%s) from input type %s", key, flags.Input.Format)
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    metadata = GetFileMD(flags.Input.Files)
    menu_json = metadata.get(key, None)
    if menu_json is None:
        menu_json = metadata.get('DataVector<xAOD::TriggerMenuJson_v1>_%s' % key, None)
    if menu_json is None:
        raise RuntimeError("Cannot read trigger configuration (%s) from input file metadata" % key)
    return menu_json


"""

L1 information

"""
def getL1MenuAccess( flags = None ):
    tc = getTrigConfigFromFlag( flags )
    if tc["SOURCE"] == "FILE":
        cfg = L1MenuAccess( filename = getL1MenuFileName( flags ) )
    elif tc["SOURCE"] == "COOL":
        """This is the case when reconstructing the data."""
        from RecExConfig.InputFilePeeker import inpSum
        keysFromCool = getKeysFromCool( inpSum["run_number"] )
        cfg = L1MenuAccess( dbalias = keysFromCool["DB"], smkey = keysFromCool['SMK'] )
    elif tc["SOURCE"] == "DB":
        cfg = L1MenuAccess( dbalias = tc["DBCONN"], smkey = tc["SMK"] )
    elif tc["SOURCE"] == "INFILE":
        cfg = L1MenuAccess(jsonString=_getJSONFromMetadata(flags, key='TriggerMenuJson_L1'))
    else:
        raise RuntimeError("Unknown source of trigger configuration: %s" % tc["SOURCE"])
    return cfg


def getL1PrescalesSetAccess( flags = None ):
    tc = getTrigConfigFromFlag( flags )
    if tc["SOURCE"] == "FILE":
        cfg = L1PrescalesSetAccess( filename = getL1PrescalesSetFileName( flags ) )
    elif tc["SOURCE"] == "COOL":
        """This is the case when reconstructing the data."""
        from RecExConfig.InputFilePeeker import inpSum
        keysFromCool = getKeysFromCool( inpSum["run_number"] )
        cfg = L1PrescalesSetAccess( dbalias = keysFromCool["DB"], l1pskey = keysFromCool['L1PSK'] )
    elif tc["SOURCE"] == "DB":
        cfg = L1PrescalesSetAccess( dbalias = tc["DBCONN"], l1pskey = tc["L1PSK"] )
    elif tc["SOURCE"] == "INFILE":
        cfg = L1PrescalesSetAccess(jsonString=_getJSONFromMetadata(flags, key='TriggerMenuJson_L1PS'))
    else:
        raise RuntimeError("Unknown source of trigger configuration: %s" % tc["SOURCE"])
    return cfg


def getBunchGroupSetAccess( flags = None ):
    tc = getTrigConfigFromFlag( flags )
    if tc["SOURCE"] == "FILE":
        cfg = BunchGroupSetAccess( filename = getBunchGroupSetFileName( flags ) )
    elif tc["SOURCE"] == "COOL":
        """This is the case when reconstructing the data."""
        from RecExConfig.InputFilePeeker import inpSum
        keysFromCool = getKeysFromCool( inpSum["run_number"] )
        cfg = BunchGroupSetAccess( dbalias = keysFromCool["DB"], bgskey = keysFromCool['BGSK'] )
    elif tc["SOURCE"] == "DB":
        cfg = BunchGroupSetAccess( dbalias = tc["DBCONN"], bgskey = tc["BGSK"] )
    elif tc["SOURCE"] == "INFILE":
        from RecExConfig.InputFilePeeker import inputFileSummary as inpSum
        if inpSum["file_type"] != 'pool':
            raise RuntimeError("Cannot read trigger configuration (Bunchgroup Set) from input type %s" % inpSum["file_type"])
        raise NotImplementedError("Python access to the trigger configuration (Bunchgroup Set) from in-file metadata not yet implemented")
    else:
        raise RuntimeError("Unknown source of trigger configuration: %s" % tc["SOURCE"])
    return cfg



"""

HLT information

"""
def getHLTMenuAccess( flags = None ):
    tc = getTrigConfigFromFlag( flags )
    if tc["SOURCE"] == "FILE":
        cfg = HLTMenuAccess( filename = getHLTMenuFileName( flags ) )
    elif tc["SOURCE"] == "COOL":
        """This is the case when reconstructing the data."""
        from RecExConfig.InputFilePeeker import inpSum
        keysFromCool = getKeysFromCool( inpSum["run_number"] )
        cfg = HLTMenuAccess( dbalias = keysFromCool["DB"], smkey = keysFromCool['SMK'] )
    elif tc["SOURCE"] == "DB":
        cfg = HLTMenuAccess( dbalias = tc["DBCONN"], smkey = tc["SMK"] )
    elif tc["SOURCE"] == "INFILE":
        cfg = HLTMenuAccess(jsonString=_getJSONFromMetadata(flags, key='TriggerMenuJson_HLT'))
    else:
        raise RuntimeError("Unknown source of trigger configuration: %s" % tc["SOURCE"])
    return cfg


def getHLTPrescalesSetAccess( flags = None ):
    tc = getTrigConfigFromFlag( flags )
    if tc["SOURCE"] == "FILE":
        cfg = HLTPrescalesSetAccess( filename = getHLTPrescalesSetFileName( flags ) )
    elif tc["SOURCE"] == "COOL":
        """This is the case when reconstructing the data."""
        from RecExConfig.InputFilePeeker import inpSum
        keysFromCool = getKeysFromCool( inpSum["run_number"] )
        cfg = HLTPrescalesSetAccess( dbalias = keysFromCool["DB"], l1pskey = keysFromCool['HLTPSK'] )
    elif tc["SOURCE"] == "DB":
        cfg = HLTPrescalesSetAccess( dbalias = tc["DBCONN"], l1pskey = tc["HLTPSK"] )
    elif tc["SOURCE"] == "INFILE":
        cfg = HLTPrescalesSetAccess(jsonString=_getJSONFromMetadata(flags, key='TriggerMenuJson_HLTPS'))
    else:
        raise RuntimeError("Unknown source of trigger configuration: %s" % tc["SOURCE"])
    return cfg


def getHLTJobOptionsAccess( flags = None ):
    tc = getTrigConfigFromFlag( flags )
    if tc["SOURCE"] == "FILE":
        cfg = HLTJobOptionsAccess( filename = getHLTJobOptionsFileName( flags ) )
    elif tc["SOURCE"] == "COOL":
        """This is the case when reconstructing the data."""
        from RecExConfig.InputFilePeeker import inpSum
        keysFromCool = getKeysFromCool( inpSum["run_number"] )
        cfg = HLTJobOptionsAccess( dbalias = keysFromCool["DB"], smkey = keysFromCool['SMK'] )
    elif tc["SOURCE"] == "DB":
        cfg = HLTJobOptionsAccess( dbalias = tc["DBCONN"], smkey = tc["SMK"] )
    elif tc["SOURCE"] == "INFILE":
        raise NotImplementedError("Python access to the HLT Job Options configuration from in-file metadata is NOT SUPPORTED (this file is huge!)")
    else:
        raise RuntimeError("Unknown source of trigger configuration: %s" % tc["SOURCE"])
    return cfg

def getHLTMonitoringAccess( flags = None ):
    tc = getTrigConfigFromFlag( flags )
    if tc["SOURCE"] == "FILE":
        cfg = HLTMonitoringAccess( filename = getHLTMonitoringFileName( flags ) )
    elif tc["SOURCE"] == "COOL":
        # TODO when database will be ready
        raise NotImplementedError("Python COOL access to the HLT monitoring not yet implemented")
    elif tc["SOURCE"] == "DB":
        # TODO when database will be ready
        raise NotImplementedError("Python DB access to the HLT monitoring not yet implemented")
    elif tc["SOURCE"] == "INFILE":
        cfg = HLTMonitoringAccess(jsonString=_getJSONFromMetadata(flags, key='TriggerMenuJson_HLTMonitoring'))
    else:
        raise RuntimeError("Unknown source of trigger configuration: %s" % tc["SOURCE"])
    return cfg
