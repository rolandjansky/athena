# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from PyUtils.Decorators import memoize
from AthenaCommon.Logging import logging
from collections import OrderedDict as odict
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
import json


@memoize
def getTrigConfFromCool(runNumber, lumiBlock):
    from TrigConfStorage.TriggerCoolUtil import TriggerCoolUtil 
    db = TriggerCoolUtil.GetConnection('CONDBR2')
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
    log = logging.getLogger('TrigConfigSvcCfg')
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

# This interprets the Trigger.triggerConfig flag according to
# https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerConfigFlag#triggerConfig_in_Run_3
def getTrigConfigFromFlag( flags ):
    tcflag = flags.Trigger.triggerConfig
    log = logging.getLogger('TrigConfigSvcCfg')
    log.info("Parsing trigger flag 'triggerConfig': %s", tcflag)
    if tcflag is None: # the default is to configure from file
        tcflag = "FILE"
    source, dbconn, keys = (tcflag+":::").split(":")[:3]
    smk,l1psk,hltpsk,bgsk = (keys+",,,").split(",")[:4]
    smk = int(smk) if smk != "" else None
    l1psk = int(l1psk) if l1psk!="" else None
    hltpsk = int(hltpsk) if hltpsk!="" else None
    bgsk = int(bgsk) if bgsk!="" else None
    if source == "DB" and (smk is None or l1psk is None or hltpsk is None or bgsk is None):
        runNumber = flags.Input.RunNumber[0]
        lbNumber = flags.Input.LumiBlockNumber[0]
        if dbconn == "":
            dbconn = getTrigConfFromCool(runNumber, lbNumber)["DB"]
        if dbconn in ["TRIGGERDBR3","TRIGGERDBR2", "TRIGGERDBDEV1", "TRIGGERDBDEV2"]:
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


# L1 Json file name 
def getL1MenuFileName(flags):
    l1MenuFileName = 'L1Menu_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    l1MenuFileName = l1MenuFileName.replace(".xml",".json").replace("LVL1config", "L1Menu")
    l1MenuFileName = l1MenuFileName.replace("_newJO","")
    return l1MenuFileName


# HLT Json file name 
def getHLTMenuFileName( flags ):
    hltMenuFileName = 'HLTMenu_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    hltMenuFileName = hltMenuFileName.replace(".xml",".json").replace("HLTconfig", "HLTMenu").replace("HLTmenu", "HLTMenu")
    hltMenuFileName = hltMenuFileName.replace("_newJO","")
    return hltMenuFileName

# L1 Prescales set json file name
def getL1PrescalesSetFileName( flags ):
    l1PrescalesSetFileName = 'L1PrescalesSet_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    return l1PrescalesSetFileName

# HLT Prescales set json file name
def getHLTPrescalesSetFileName( flags ):
    hltPrescalesSetFileName = 'HLTPrescalesSet_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    return hltPrescalesSetFileName

# HLT Monitoring set json file name
def getHLTMonitoringFileName( flags ):
    hltMonitoringFileName = 'HLTMonitoring_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    return hltMonitoringFileName


# L1 Bunchgroups set json file name
def getBunchGroupSetFileName( flags ):
    bunchGroupSetFileName = 'BunchGroupSet_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    return bunchGroupSetFileName

# HLT Job options json file name
def getHLTJobOptionsFileName( ):
    return 'HLTJobOptions.json'

# Creates an L1 Prescale file from the menu
# this is a temporary solution, in the final version the L1PrescalesSet file should come from the menu
def createL1PrescalesFileFromMenu( flags ):
    log = logging.getLogger('TrigConfigSvcCfg')
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
    tcflag = flags.Trigger.triggerConfig
    if tcflag is None:
        tcflag = "FILE"
    source = (tcflag+":::").split(":")[0]
    if source=="FILE":
        menuFileName = getL1MenuFileName(flags)
        bgsFileName = getBunchGroupSetFileName(flags)
        return _generateL1Menu(flags.Trigger.triggerMenuSetup, menuFileName, bgsFileName)
    return None, None

@memoize
def _generateL1Menu(triggerMenuSetup, fileName, bgsFileName):
    log = logging.getLogger('TrigConfigSvcCfg')
    log.info("Generating L1 menu %s", triggerMenuSetup)
    from TriggerMenuMT.L1.L1MenuConfig import L1MenuConfig
    l1cfg = L1MenuConfig( menuName = triggerMenuSetup)
    outfile, bgsOutFile = l1cfg.writeJSON(outputFile = fileName, bgsOutputFile = bgsFileName)
    if outfile is not None:
        log.info("Wrote L1 menu file %s", outfile)
    if bgsOutFile is not None:
        log.info("Wrote bunchgroup set file %s", bgsOutFile)

    return outfile, bgsOutFile


# configuration of L1ConfigSvc
@memoize
def getL1TopoConfigSvc( flags ):
    log = logging.getLogger('TrigConfigSvcCfg')
    # configure config svc
    TrigConf__L1TopoConfigSvc = CompFactory.getComp("TrigConf::L1TopoConfigSvc")
    l1topoConfigSvc = TrigConf__L1TopoConfigSvc("L1TopoConfigSvc")

    l1topoConfigSvc.ConfigSource = "XML"
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    l1topoXMLFile = TriggerFlags.inputL1TopoConfigFile() if flags is None else flags.Trigger.LVL1TopoConfigFile
    # check if file exists in this directory otherwise add the package to aid path resolution
    # also a '/' in the file name indicates that no package needs to be added
    import os.path
    if not ( "/" in l1topoXMLFile or os.path.isfile(l1topoXMLFile) ):
        l1topoXMLFile = "TriggerMenuMT/" + l1topoXMLFile
    l1topoConfigSvc.XMLMenuFile = l1topoXMLFile
    log.info( "Configured L1TopoConfigSvc with input file : %s", l1topoXMLFile )

    from AthenaCommon.AppMgr import theApp
    theApp.CreateSvc += [ "TrigConf::L1TopoConfigSvc/L1TopoConfigSvc" ]
    return l1topoConfigSvc


# configuration of L1ConfigSvc
@memoize
def getL1ConfigSvc( flags ):
    log = logging.getLogger('TrigConfigSvcCfg')
    # generate menu file (this only happens if we read from FILE)
    generatedFile, generatedBgsFile = generateL1Menu( flags )

    cfg = getTrigConfigFromFlag( flags )
    log.info( "Configure LVL1ConfigSvc" )

    # configure config svc
    TrigConf__LVL1ConfigSvc = CompFactory.getComp("TrigConf::LVL1ConfigSvc")
    l1ConfigSvc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")

    if cfg["SOURCE"] == "FILE":
        # Run 2 configuration
        l1ConfigSvc.ConfigSource = "XML"
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        l1XMLFile = TriggerFlags.inputLVL1configFile() if flags is None else flags.Trigger.LVL1ConfigFile
        # check if file exists in this directory otherwise add the package to aid path resolution
        # also a '/' in the file name indicates that no package needs to be added
        import os.path
        if not ( "/" in l1XMLFile or os.path.isfile(l1XMLFile) ):
            l1XMLFile = "TriggerMenuMT/" + l1XMLFile
        l1ConfigSvc.XMLMenuFile = l1XMLFile
        log.info( "For run 2 style menu access configured LVL1ConfigSvc with input file : %s", l1XMLFile )
        # Run 3 configuration
        l1ConfigSvc.InputType = "file"
        l1ConfigSvc.JsonFileName = generatedFile
        l1ConfigSvc.JsonFileNameBGS = generatedBgsFile
        log.info( "For run 3 style menu access configured LVL1ConfigSvc with InputType='file', JsonFileName=%s and JsonFileNameBGS=%s", generatedFile, generatedBgsFile )
    elif cfg["SOURCE"] == "DB":
        l1ConfigSvc.ConfigSource = "none"
        l1ConfigSvc.InputType = "DB"
        l1ConfigSvc.JsonFileName = ""
        l1ConfigSvc.TriggerDB = cfg["DBCONN"]
        l1ConfigSvc.SMK = cfg["SMK"]
        l1ConfigSvc.BGSK = cfg["BGSK"]
        log.info( "For run 3 style menu access configured LVL1ConfigSvc with InputType='DB', SMK %d, and BGSK %d", cfg['SMK'], cfg['BGSK']  )

    from AthenaCommon.AppMgr import theApp
    theApp.CreateSvc += [ "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc" ]
    return l1ConfigSvc


# configuration of HLTConfigSvc
@memoize
def getHLTConfigSvc( flags ):
    log = logging.getLogger('TrigConfigSvcCfg')
    cfg = getTrigConfigFromFlag( flags )
    log.info( "Configure HLTConfigSvc" )

    TrigConf__HLTConfigSvc = CompFactory.getComp("TrigConf::HLTConfigSvc")
    hltConfigSvc = TrigConf__HLTConfigSvc("HLTConfigSvc")

    if cfg["SOURCE"] == "FILE":
        hltXMLFile = "None"
        hltConfigSvc.ConfigSource = "None"
        hltConfigSvc.XMLMenuFile = hltXMLFile
        hltConfigSvc.InputType = "file"
        hltJsonFileName = getHLTMenuFileName( flags )
        hltConfigSvc.JsonFileName = hltJsonFileName
        # TODO revisit if needed    
        log.info( "Configured HLTConfigSvc with run 2 style input file : %s", hltXMLFile  )
        log.info( "Configured HLTConfigSvc with InputType='file' and JsonFileName=%s", hltJsonFileName )
    elif cfg["SOURCE"] == "DB":
        hltConfigSvc.ConfigSource = "none"
        hltConfigSvc.InputType = "DB"
        hltConfigSvc.JsonFileName = ""
        hltConfigSvc.TriggerDB = cfg["DBCONN"]
        hltConfigSvc.SMK = cfg["SMK"]
        log.info( "For run 3 style menu access configured HLTConfigSvc with InputType='DB' and SMK %d", cfg['SMK'] )

    from AthenaCommon.AppMgr import theApp
    theApp.CreateSvc += [ "TrigConf::HLTConfigSvc/HLTConfigSvc" ]
    return hltConfigSvc


# provide L1 config service in new JO
def L1ConfigSvcCfg( flags ):
    acc = ComponentAccumulator()
    acc.addService( getL1ConfigSvc( flags ) )
    return acc

# provide HLT config service in new JO
def HLTConfigSvcCfg( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    acc.addService( getHLTConfigSvc( flags ) )
    return acc

# provide both services in new JO
def TrigConfigSvcCfg( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    acc.addService( getL1ConfigSvc( flags ), create=True )
    acc.addService( getHLTConfigSvc( flags ), create=True )
    return acc

def L1PrescaleCondAlgCfg( flags ):
    log = logging.getLogger('TrigConfigSvcCfg')
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
    else:
        raise RuntimeError("trigger configuration flag 'trigConfig' starts with %s, which is not understood" % tc["SOURCE"])
    acc.addCondAlgo(l1PrescaleCondAlg)
    return acc

def HLTPrescaleCondAlgCfg( flags ):
    log = logging.getLogger('TrigConfigSvcCfg')
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
    else:
        raise RuntimeError("trigger configuration flag 'trigConfig' starts with %s, which is not understood" % tc["SOURCE"])
    acc.addCondAlgo(hltPrescaleCondAlg)
    return acc

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.lock()
    acc = TrigConfigSvcCfg( ConfigFlags )
    acc.store( open( "test.pkl", "wb" ) )
    print("All OK")
