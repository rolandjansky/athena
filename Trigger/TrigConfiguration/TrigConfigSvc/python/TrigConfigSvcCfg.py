# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AccumulatorCache import AccumulatorCache

from collections import OrderedDict as odict
from functools import cache
import json

log = logging.getLogger('TrigConfigSvcCfg')

# To avoid accidental overrwrite of the L1 JSON menu, only allow one L1 menu generation.
# Either via JSON conversion from Run-1&2 or native Run-3 (see ATR-24531).
def l1menu_generated():
    try:
        return l1menu_generated._hasRun
    except AttributeError:
        l1menu_generated._hasRun = True
        return False


@cache
def getTrigConfFromCool(runNumber, lumiBlock):
    from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil 
    db = TriggerCoolUtil.GetConnection('CONDBR2' if runNumber > 230000 else 'COMP200')
    runRange = [[runNumber,runNumber]]
    d = {key: value for key, value in TriggerCoolUtil.getHLTConfigKeys(db, runRange)[runNumber].items() if  key in ["SMK", "DB"]}
    d["DB"] = d["DB"].split(';')[0]
    for (hltpsk, firstlb, lastlb) in TriggerCoolUtil.getHLTPrescaleKeys(db, runRange)[runNumber]['HLTPSK2']:
        if firstlb<=lumiBlock and lumiBlock<=lastlb:
            d['HLTPSK'] = hltpsk
            break
    for (l1psk, firstlb, lastlb) in TriggerCoolUtil.getL1ConfigKeys(db, runRange)[runNumber]['LVL1PSK']:
        if firstlb<=lumiBlock and lumiBlock<=lastlb:
            d['L1PSK'] = l1psk
            break
    for (bgsk, firstlb, lastlb) in TriggerCoolUtil.getBunchGroupKey(db, runRange)[runNumber]['BGKey']:
        if firstlb<=lumiBlock and lumiBlock<=lastlb:
            d['BGSK'] = bgsk
            break

    if 'L1PSK' not in d:
        msg = f"Did not find an L1 PSK for run {runNumber} and lumi block {lumiBlock} in COOL"
        log.error(msg)
        raise RuntimeError(msg)
    if 'HLTPSK' not in d:
        msg = f"Did not find an HLT PSK for run {runNumber} and lumi block {lumiBlock} in COOL"
        log.error(msg)
        raise RuntimeError(msg)
    if 'BGSK' not in d:
        msg = f"Did not find a bunch group set key for run {runNumber} and lumi block {lumiBlock} in COOL"
        log.error(msg)
        raise RuntimeError(msg)
    log.info("Extracted the following info for run %d and lumi block %d from COOL: %r", runNumber, lumiBlock, d)
    return d


@cache
def createJsonMenuFiles(run, lb):
    """Retrieve Run-2 trigger configuration from the DB and save as Run3 .JSON files"""
    import subprocess

    if l1menu_generated():
        log.error("L1 menu has already been generated")
        return None

    log.info("Configuring Run-1&2 to Run-3 configuration metadata conversion")
    triggerDBKeys = getTrigConfFromCool(run, lb)
    triggerDBKeys['DB'] = 'TRIGGERDB' if run > 230000 else 'TRIGGERDB_RUN1'

    cmd = "TrigConfReadWrite -i {DB} {SMK},{L1PSK},{HLTPSK},{BGSK} -o r3json > Run3ConfigFetchJSONFiles.log".format(**triggerDBKeys)
    log.info("Running command '%s'", cmd)
    filesFetchStatus = subprocess.run(cmd, shell=True)
    assert filesFetchStatus.returncode == 0, "TrigConfReadWrite failed to fetch JSON files"
    return triggerDBKeys


# This interprets the Trigger.triggerConfig flag according to
# https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerConfigFlag#triggerConfig_in_Run_3
def getTrigConfigFromFlag( flags ):
    # Pad the triggerConfig value and extract available fields:
    source, dbconn, keys = (flags.Trigger.triggerConfig+":::").split(":")[:3]
    smk,l1psk,hltpsk,bgsk = (keys+",,,").split(",")[:4]
    # Convert to int or None:
    smk, l1psk, hltpsk, bgsk = (int(k) if k!="" else None for k in (smk, l1psk, hltpsk, bgsk))

    if source == "DB" and (smk is None or l1psk is None or hltpsk is None or bgsk is None):
        runNumber = flags.Input.RunNumber[0]
        lbNumber = flags.Input.LumiBlockNumber[0]
        if dbconn == "":
            dbconn = getTrigConfFromCool(runNumber, lbNumber)["DB"]
        if dbconn in ["TRIGGERDB_RUN3", "TRIGGERDBDEV1_I8", "TRIGGERDBDEV1", "TRIGGERDBDEV2"]:
            d = getTrigConfFromCool(runNumber, lbNumber)            
            if smk is None:
                smk = d["SMK"]
            if l1psk is None:
                l1psk = d['L1PSK']
            if hltpsk is None:
                hltpsk = d['HLTPSK']
            if bgsk is None:
                bgsk = d['BGSK']

    tcdict = {
        "SOURCE" : source.upper(),  # DB, FILE, COOL
        "DBCONN" : dbconn, # db connection (if origin==DB or COOL) or "JOSVC" if connection is to be taken from TrigConf::IJobOptionsSvc 
        "SMK"    : smk,
        "L1PSK"  : l1psk,
        "HLTPSK" : hltpsk,
        "BGSK"   : bgsk
    }
    return tcdict


def getL1PrescaleFolderName():
    return "/TRIGGER/LVL1/Lvl1ConfigKey <tag>HEAD</tag>"


def getHLTPrescaleFolderName():
    return "/TRIGGER/HLT/PrescaleKey <tag>HEAD</tag>"


def _doMenuConversion(flags):
    """Do JSON menu conversion for Run-1&2 data"""
    return flags.Input.Files and flags.Trigger.EDMVersion in [1, 2] and not flags.Input.isMC


def _getMenuFileName(flags):
    """Return base name for menu files"""
    if not _doMenuConversion(flags):  # menu created in this release
        from AthenaCommon.AppMgr import release_metadata
        return '_'+flags.Trigger.triggerMenuSetup+'_'+release_metadata()['release']
    else:  # menu files created via JSON conversion
        return ''

# L1 Json file name 
def getL1MenuFileName(flags):
    l1MenuFileName = 'L1Menu'+_getMenuFileName(flags)+'.json'
    l1MenuFileName = l1MenuFileName.replace("_newJO","")
    return l1MenuFileName

# HLT Json file name 
def getHLTMenuFileName( flags ):
    hltMenuFileName = 'HLTMenu'+_getMenuFileName(flags)+'.json'
    hltMenuFileName = hltMenuFileName.replace("_newJO","")
    return hltMenuFileName

# L1 Prescales set json file name
def getL1PrescalesSetFileName( flags ):
    return 'L1PrescalesSet'+_getMenuFileName(flags)+'.json'

# HLT Prescales set json file name
def getHLTPrescalesSetFileName( flags ):
    return 'HLTPrescalesSet'+_getMenuFileName(flags)+'.json'

# HLT Monitoring set json file name
def getHLTMonitoringFileName( flags ):
    return 'HLTMonitoring'+_getMenuFileName(flags)+'.json'

# L1 Bunchgroups set json file name
def getBunchGroupSetFileName( flags ):
    return 'BunchGroupSet'+_getMenuFileName(flags)+'.json'

# HLT Job options json file name
def getHLTJobOptionsFileName( ):
    return 'HLTJobOptions.json'

# Creates an L1 Prescale file from the menu
# this is a temporary solution, in the final version the L1PrescalesSet file should come from the menu
def createL1PrescalesFileFromMenu( flags ):
    menuFN = getL1MenuFileName(flags)
    with open(menuFN,'r') as fh:
        data = json.load(fh)
        pso = odict()
        pso['filetype'] = 'l1prescale'
        pso['name'] = data['name']
        pso['cutValues'] = odict()
        ps = pso['cutValues']
        for name, item in sorted(data['items'].items()):
            ps[name] = odict([
                ("cut", 1),
                ("enabled", True),
                ("info", "prescale: 1")
            ])
    psFN = getL1PrescalesSetFileName( flags )
    with open(psFN, 'w') as outfile:
        json.dump(pso, outfile, indent = 4)
        log.info("Generated default L1 prescale set %s", outfile.name)

# L1 menu generation
def generateL1Menu( flags ):
    if l1menu_generated():
        log.error("L1 menu has already been generated")
        return

    log.info("Generating L1 menu %s", flags.Trigger.triggerMenuSetup)
    from TriggerMenuMT.L1.L1MenuConfig import L1MenuConfig
    l1cfg = L1MenuConfig(menuName = flags.Trigger.triggerMenuSetup)
    l1cfg.writeJSON(outputFile    = getL1MenuFileName(flags),
                    bgsOutputFile = getBunchGroupSetFileName(flags))


# provide L1 config service in new JO
@AccumulatorCache
def L1ConfigSvcCfg( flags ):
    acc = ComponentAccumulator()

    cfg = getTrigConfigFromFlag( flags )
    log.info( "Configure LVL1ConfigSvc" )

    # configure config svc
    l1ConfigSvc = CompFactory.getComp("TrigConf::LVL1ConfigSvc")("LVL1ConfigSvc")

    if cfg["SOURCE"] == "FILE":
        if _doMenuConversion(flags):
            # Save the menu in JSON format
            dbKeys = createJsonMenuFiles(run = flags.Input.RunNumber[0],
                                         lb = flags.Input.LumiBlockNumber[0])
            l1ConfigSvc.SMK = dbKeys['SMK']
            l1ConfigSvc.BGSK = dbKeys['BGSK']

        l1ConfigSvc.InputType = "FILE"
        l1ConfigSvc.JsonFileName = getL1MenuFileName(flags)
        l1ConfigSvc.JsonFileNameBGS  = getBunchGroupSetFileName(flags)
        log.info( "For run 3 style menu access configured LVL1ConfigSvc with InputType='FILE', JsonFileName=%s and JsonFileNameBGS=%s", l1ConfigSvc.JsonFileName, l1ConfigSvc.JsonFileNameBGS )
    elif cfg["SOURCE"] == "DB":
        l1ConfigSvc.InputType = "DB"
        l1ConfigSvc.JsonFileName = ""
        l1ConfigSvc.TriggerDB = cfg["DBCONN"]
        l1ConfigSvc.SMK = cfg["SMK"]
        l1ConfigSvc.BGSK = cfg["BGSK"]
        log.info( "For run 3 style menu access configured LVL1ConfigSvc with InputType='DB', SMK %d, and BGSK %d", cfg['SMK'], cfg['BGSK'] )

    acc.addService( l1ConfigSvc, create=True )
    return acc

# provide HLT config service in new JO
@AccumulatorCache
def HLTConfigSvcCfg( flags ):
    acc = ComponentAccumulator()
    cfg = getTrigConfigFromFlag( flags )
    log.info( "Configure HLTConfigSvc" )

    hltConfigSvc = CompFactory.getComp("TrigConf::HLTConfigSvc")("HLTConfigSvc")

    if cfg["SOURCE"] == "FILE":
        if _doMenuConversion(flags):
            # Save the menu in JSON format
            dbKeys = createJsonMenuFiles(run = flags.Input.RunNumber[0],
                                         lb = flags.Input.LumiBlockNumber[0])
            hltConfigSvc.SMK = dbKeys['SMK']

        hltConfigSvc.InputType = "FILE"
        hltConfigSvc.JsonFileName = getHLTMenuFileName( flags )
        log.info( "Configured HLTConfigSvc with InputType='FILE' and JsonFileName=%s", hltConfigSvc.JsonFileName )
    elif cfg["SOURCE"] == "DB":
        hltConfigSvc.InputType = "DB"
        hltConfigSvc.JsonFileName = ""
        hltConfigSvc.TriggerDB = cfg["DBCONN"]
        hltConfigSvc.SMK = cfg["SMK"]
        log.info( "For run 3 style menu access configured HLTConfigSvc with InputType='DB' and SMK %d", cfg['SMK'] )
    acc.addService( hltConfigSvc, create=True )
    return acc

# provide both services in new JO
def TrigConfigSvcCfg( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    acc.merge( L1ConfigSvcCfg( flags ) )
    acc.merge( HLTConfigSvcCfg( flags ) )
    return acc

def L1PrescaleCondAlgCfg( flags ):
    log.info("Setting up L1PrescaleCondAlg")
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    TrigConf__L1PrescaleCondAlg = CompFactory.getComp("TrigConf::L1PrescaleCondAlg")
    l1PrescaleCondAlg = TrigConf__L1PrescaleCondAlg("L1PrescaleCondAlg")

    tc = getTrigConfigFromFlag( flags )
    l1PrescaleCondAlg.Source = tc["SOURCE"]
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline():
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        acc.merge(addFolders(flags, getL1PrescaleFolderName(), "TRIGGER_ONL", className="AthenaAttributeList"))
        log.info("Adding folder %s to CompAcc", getL1PrescaleFolderName() )
    if tc["SOURCE"] == "COOL":
        l1PrescaleCondAlg.TriggerDB = tc["DBCONN"]
    elif tc["SOURCE"] == "DB":
        l1PrescaleCondAlg.TriggerDB = tc["DBCONN"]
        l1PrescaleCondAlg.L1Psk    = tc["L1PSK"]
    elif tc["SOURCE"] == "FILE":
        l1PrescaleCondAlg.Filename = getL1PrescalesSetFileName( flags )
        if _doMenuConversion(flags):
            # Save the menu in JSON format
            dbKeys = createJsonMenuFiles(run = flags.Input.RunNumber[0],
                                         lb = flags.Input.LumiBlockNumber[0])
            l1PrescaleCondAlg.L1Psk = dbKeys['L1PSK']
    else:
        raise RuntimeError("trigger configuration flag 'trigConfig' starts with %s, which is not understood" % tc["SOURCE"])
    acc.addCondAlgo(l1PrescaleCondAlg)
    return acc

def HLTPrescaleCondAlgCfg( flags ):
    log.info("Setting up HLTPrescaleCondAlg")
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    TrigConf__HLTPrescaleCondAlg = CompFactory.getComp("TrigConf::HLTPrescaleCondAlg")
    hltPrescaleCondAlg = TrigConf__HLTPrescaleCondAlg("HLTPrescaleCondAlg")

    tc = getTrigConfigFromFlag( flags )
    hltPrescaleCondAlg.Source = tc["SOURCE"]
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline() or tc["SOURCE"]=="COOL":
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        acc.merge(addFolders(flags, getHLTPrescaleFolderName(), "TRIGGER_ONL", className="AthenaAttributeList"))
        log.info("Adding folder %s to CompAcc", getHLTPrescaleFolderName() )
    if tc["SOURCE"] == "COOL":
        hltPrescaleCondAlg.TriggerDB = tc["DBCONN"]
    elif tc["SOURCE"] == "DB":
        hltPrescaleCondAlg.TriggerDB = tc["DBCONN"]
        hltPrescaleCondAlg.HLTPsk    = tc["HLTPSK"]
    elif tc["SOURCE"] == "FILE":
        hltPrescaleCondAlg.Filename = getHLTPrescalesSetFileName( flags )
        if _doMenuConversion(flags):
            # Save the menu in JSON format
            dbKeys = createJsonMenuFiles(run = flags.Input.RunNumber[0],
                                         lb = flags.Input.LumiBlockNumber[0])
            hltPrescaleCondAlg.HLTPsk = dbKeys['HLTPSK']
    else:
        raise RuntimeError("trigger configuration flag 'trigConfig' starts with %s, which is not understood" % tc["SOURCE"])
    acc.addCondAlgo(hltPrescaleCondAlg)
    return acc


if __name__ == "__main__":
    import unittest

    class Tests(unittest.TestCase):

        def setUp(self):
            # Allow multiple L1 menu generations for these tests
            l1menu_generated._hasRun = False

        def test_currentMenu(self):
            from AthenaConfiguration.AllConfigFlags import _createCfgFlags
            ConfigFlags = _createCfgFlags()
            ConfigFlags.Trigger.EDMVersion = 3
            from AthenaConfiguration.TestDefaults import defaultTestFiles
            ConfigFlags.Input.Files = defaultTestFiles.RAW
            ConfigFlags.lock()
            TrigConfigSvcCfg( ConfigFlags )
            L1PrescaleCondAlgCfg( ConfigFlags )
            HLTPrescaleCondAlgCfg( ConfigFlags )

        def test_legacyMenu(self):
            from AthenaConfiguration.AllConfigFlags import _createCfgFlags
            ConfigFlags = _createCfgFlags()
            from AthenaConfiguration.TestDefaults import defaultTestFiles
            ConfigFlags.Input.Files = defaultTestFiles.RAW
            ConfigFlags.lock()
            TrigConfigSvcCfg( ConfigFlags )
            L1PrescaleCondAlgCfg( ConfigFlags )
            HLTPrescaleCondAlgCfg( ConfigFlags )

        def test_jsonConverter(self):
            keys = createJsonMenuFiles(run=360026, lb=151)
            for k,v in {"SMK" : 2749, "L1PSK" : 23557, "HLTPSK" : 17824, "BGSK" : 2181}.items():
                assert  k in keys, "Missing key {}".format(k)
                assert v == keys[k], "Wrong value {}".format(v)

    unittest.main(verbosity=2)
