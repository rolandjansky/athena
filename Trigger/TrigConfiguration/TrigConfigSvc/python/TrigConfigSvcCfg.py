# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from PyUtils.Decorators import memoize
from AthenaCommon.Logging import logging

# L1 Json file name 
def getL1MenuFileName( flags=None ):
    if flags is None:
        from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
        l1MenuFileName = 'L1Menu_'+tf.triggerMenuSetup()+'_'+tf.menuVersion()+'.json'
    else:
        l1MenuFileName = 'L1Menu_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    l1MenuFileName = l1MenuFileName.replace(".xml",".json").replace("LVL1config", "L1Menu")
    l1MenuFileName = l1MenuFileName.replace("_newJO","")
    return l1MenuFileName

# HLT Json file name 
def getHLTMenuFileName( flags=None ):
    if flags is None:
        from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
        hltMenuFileName = 'HLTMenu_'+tf.triggerMenuSetup()+'_'+tf.menuVersion()+'.json'
    else:
        hltMenuFileName = 'HLTMenu_'+flags.Trigger.triggerMenuSetup+'_'+flags.Trigger.menuVersion+'.json'
    hltMenuFileName = hltMenuFileName.replace(".xml",".json").replace("HLTconfig", "HLTMenu").replace("HLTmenu", "HLTMenu")
    hltMenuFileName = hltMenuFileName.replace("_newJO","")
    return hltMenuFileName


# L1 menu generation
@memoize
def generateL1Menu( flags=None ):
    log = logging.getLogger('TrigConfigSvcCfg')
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    fileName = getL1MenuFileName( flags )
    if flags:
        TriggerFlags.triggerMenuSetup = flags.Trigger.triggerMenuSetup
    menuName = TriggerFlags.triggerMenuSetup() if flags is None else flags.Trigger.triggerMenuSetup
    log.info("Generating L1 menu %s", menuName)
    from TriggerMenuMT.L1.L1MenuConfig import L1MenuConfig
    l1cfg = L1MenuConfig( menuName = menuName )
    outfile = l1cfg.writeJSON(outputFile = fileName)
    if outfile is not None:
        log.info("Wrote L1 menu file %s", outfile)
    return outfile


# configuration of L1ConfigSvc
@memoize
def getL1ConfigSvc( flags = None ):
    log = logging.getLogger('TrigConfigSvcCfg')
    from AthenaCommon.Logging import log
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
    from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
    from AthenaCommon.AppMgr import theApp
    # generate menu file
    generatedFile = generateL1Menu( flags=flags )

    # configure config svc
    l1ConfigSvc = TrigConf__LVL1ConfigSvc( "LVL1ConfigSvc" )

    l1ConfigSvc.ConfigSource = "XML"
    l1XMLFile = findFileInXMLPATH( TriggerFlags.inputLVL1configFile() if flags is None else flags.Trigger.LVL1ConfigFile )
    l1ConfigSvc.XMLMenuFile = l1XMLFile
    log.info( "Configured LVL1ConfigSvc with run 2 style input file : %s", l1XMLFile )

    if generatedFile is None:
        l1ConfigSvc.InputType = "none"
        l1ConfigSvc.JsonFileName = ""
        log.info( "Configured LVL1ConfigSvc with InputType='none'" )
    else:
        l1ConfigSvc.InputType = "file"
        l1JsonFileName = generatedFile
        l1ConfigSvc.JsonFileName = l1JsonFileName
        log.info( "Configured LVL1ConfigSvc with InputType='file' and JsonFileName=%s", l1JsonFileName )

    theApp.CreateSvc += [ "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc" ]
    return l1ConfigSvc


# configuration of HLTConfigSvc
@memoize
def getHLTConfigSvc( flags = None ):
    log = logging.getLogger('TrigConfigSvcCfg')
    from AthenaCommon.Logging import log
    from TrigConfigSvc.TrigConfigSvcConf import TrigConf__HLTConfigSvc
    from AthenaCommon.AppMgr import theApp
    hltConfigSvc = TrigConf__HLTConfigSvc( "HLTConfigSvc" )
    hltXMLFile = "None"
    hltConfigSvc.ConfigSource = "Run3_Dummy"
    hltConfigSvc.XMLMenuFile = hltXMLFile
    hltConfigSvc.InputType = "file"
    hltJsonFileName = getHLTMenuFileName( flags )
    hltConfigSvc.JsonFileName = hltJsonFileName
    theApp.CreateSvc += [ "TrigConf::HLTConfigSvc/HLTConfigSvc" ]
    log.info( "Configured HLTConfigSvc with run 2 style input file : %s", hltXMLFile  )
    log.info( "Configured HLTConfigSvc with InputType='file' and JsonFileName=%s", hltJsonFileName )
    return hltConfigSvc

# provide L1 config service in new JO
def L1ConfigSvcCfg( flags ):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
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
    acc.addService( getL1ConfigSvc( flags ) )
    acc.addService( getHLTConfigSvc( flags ) )
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True    

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.lock()
    acc = TrigConfigSvcCfg( ConfigFlags )
    acc.store( file( "test.pkl", "w" ) )
    print("All OK")
