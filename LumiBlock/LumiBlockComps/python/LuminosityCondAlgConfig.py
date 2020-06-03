# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: LumiBlockComps/python/LuminosityCondAlgConfig.py
# Created: May 2019, sss, from existing LuminosityToolDefault.
# Purpose: Configure LuminosityCondAlg.
#

from __future__ import print_function

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from AthenaCommon.Logging import logging


def LuminosityCondAlgCfg (configFlags, useOnlineLumi=None, suffix=None):
    # This function, without the useOnlineLumi and suffix arguments,  will set up a default configuration 
    # appropriate to the job; the conditions object will be called LuminosityCondData
    # Can override whether it sets up the online lumi (but then you are strongly urged to use the suffix
    # argument!)
    # Suffix will allow you to make an object with a different name, e.g. LuminosityCondDataOnline
    log = logging.getLogger ('LuminosityCondAlgCfg')
    name = 'LuminosityCondAlg'
    result = ComponentAccumulator()

    if suffix is None: suffix = ''
    if useOnlineLumi is not None and suffix is None:
        log.warning('useOnlineLumi argument is provided but not a suffix for the lumi object name. Is this really intended?')

    if configFlags.Input.isMC:
        log.info ("LuminosityCondAlgCfg called for MC!")
        kwargs = luminosityCondAlgMCCfg (configFlags, name, result)
    elif configFlags.Beam.Type != 'collisions':
        kwargs = luminosityCondAlgCosmicsCfg (configFlags, name, result)
    elif ((useOnlineLumi is None and configFlags.Common.useOnlineLumi)
          or (useOnlineLumi is not None and useOnlineLumi)):
        kwargs = luminosityCondAlgOnlineCfg (configFlags, name, result)
    elif configFlags.IOVDb.DatabaseInstance == 'COMP200':
        kwargs = luminosityCondAlgRun1Cfg (configFlags, name, result)
    elif configFlags.IOVDb.DatabaseInstance == 'CONDBR2':
        kwargs = luminosityCondAlgRun2Cfg (configFlags, name, result)

    else:
        log.warning ("LuminosityCondAlgCfg can't resolve database instance = %s, assume Run2!" % configFlags.IOVDb.DatabaseInstance)
        kwargs = luminosityCondAlgRun2Cfg (configFlags, name, result)

    LuminosityCondAlg=CompFactory.LuminosityCondAlg

    alg = LuminosityCondAlg (name,
                             LuminosityOutputKey = 'LuminosityCondData' + suffix,
                             **kwargs)

    result.addCondAlgo (alg)
    return result


def luminosityCondAlgMCCfg (configFlags, name, result):
    return { 'LuminosityFolderInputKey' : '',
             'OnlineLumiCalibrationInputKey' : '',
             'BunchLumisInputKey' : '',
             'BunchGroupInputKey' : '',
             'FillParamsInputKey' : '' }
             
def luminosityCondAlgCosmicsCfg (configFlags, name, result):
    return { 'LuminosityFolderInputKey' : '',
             'OnlineLumiCalibrationInputKey' : '',
             'BunchLumisInputKey' : '',
             'BunchGroupInputKey' : '',
             'FillParamsInputKey' : '' }

# Configuration for offline default luminosity used in Run2
def luminosityCondAlgRun2Cfg (configFlags, name, result):
    log = logging.getLogger(name)

    kwargs = {}

    # Check if this is express stream or bulk
    if configFlags.Common.doExpressProcessing:
        lumiFolder  = "/TRIGGER/LUMI/OnlPrefLumi"
        result.merge (addFolders (configFlags, lumiFolder, 'TRIGGER_ONL',
                                  className = 'CondAttrListCollection'))

    else:
        lumiFolder = "/TRIGGER/OFLLUMI/OflPrefLumi"
        result.merge (addFolders (configFlags, lumiFolder, 'TRIGGER_OFL',
                                  className = 'CondAttrListCollection'))

    log.info ("luminosityCondAlgRun2Config requested %s", lumiFolder)
    kwargs['LuminosityFolderInputKey'] = lumiFolder

    log.info ("Created Run2 %s using folder %s" % (name, lumiFolder))

    # Need the calibration just to get the proper MuToLumi value
    from CoolLumiUtilities.OnlineLumiCalibrationCondAlgConfig \
        import OnlineLumiCalibrationCondAlgCfg
    result.merge (OnlineLumiCalibrationCondAlgCfg(configFlags))
    olalg = result.getCondAlgo ('OnlineLumiCalibrationCondAlg')
    kwargs['OnlineLumiCalibrationInputKey'] = olalg.LumiCalibOutputKey
    
    # Other folder names should be blank.
    kwargs['BunchLumisInputKey'] = ''
    kwargs['BunchGroupInputKey'] = ''
    kwargs['FillParamsInputKey'] = ''

    return kwargs


# Configuration for offline default luminosity used in Run1
def luminosityCondAlgRun1Cfg (configFlags, name, result):
    log = logging.getLogger(name)

    kwargs = {}
       
    # Check if this is express stream or bulk
    if configFlags.Common.doExpressProcessing:
        lumiFolder  = "/TRIGGER/LUMI/LBLESTONL"
        result.merge (addFolders (configFlags, lumiFolder, 'TRIGGER_ONL',
                                  className = 'CondAttrListCollection'))

    else:
        lumiFolder = "/TRIGGER/OFLLUMI/LBLESTOFL"
        result.merge (addFolders (configFlags, lumiFolder, 'TRIGGER_OFL',
                                  className = 'CondAttrListCollection'))

    log.info ("configureLuminosityCondAlgRun1 requested %s", lumiFolder)
    kwargs['LuminosityFolderInputKey'] = lumiFolder

    # Configure input conditions data.
    from CoolLumiUtilities.FillParamsCondAlgConfig \
        import FillParamsCondAlgCfg
    result.merge (FillParamsCondAlgCfg (configFlags))
    fpalg = result.getCondAlgo ('FillParamsCondAlg')
    kwargs['FillParamsInputKey'] = fpalg.FillParamsOutputKey

    from CoolLumiUtilities.BunchLumisCondAlgConfig \
        import BunchLumisCondAlgCfg
    result.merge (BunchLumisCondAlgCfg (configFlags))
    blalg = result.getCondAlgo ('BunchLumisCondAlg')
    kwargs['BunchLumisInputKey'] = blalg.BunchLumisOutputKey

    from CoolLumiUtilities.BunchGroupCondAlgConfig \
        import BunchGroupCondAlgCfg
    result.merge (BunchGroupCondAlgCfg (configFlags))
    bgalg = result.getCondAlgo ('BunchGroupCondAlg')
    kwargs['BunchGroupInputKey'] = bgalg.BunchGroupOutputKey

    from CoolLumiUtilities.OnlineLumiCalibrationCondAlgConfig \
        import OnlineLumiCalibrationCondAlgCfg
    result.merge (OnlineLumiCalibrationCondAlgCfg (configFlags))
    olalg = result.getCondAlgo ('OnlineLumiCalibrationCondAlg')
    kwargs['OnlineLumiCalibrationInputKey'] = olalg.LumiCalibOutputKey

    return kwargs



# Configuration for online luminosity.
def luminosityCondAlgOnlineCfg (configFlags, name, result):
    log = logging.getLogger(name)

    kwargs = {}

    # Keep values for invalid data
    kwargs['SkipInvalid'] = False

    if configFlags.IOVDb.DatabaseInstance == 'COMP200': # Run1
        folder  = "/TRIGGER/LUMI/LBLESTONL"
        result.merge (addFolders (configFlags, folder, 'TRIGGER_ONL',
                                  className = 'CondAttrListCollection'))
      
    else: #  Run 2
        if configFlags.IOVDb.DatabaseInstance != 'CONDBR2':
            log.warning("LuminosityCondAlgOnlineCfg can't resolve DatabaseInstance = %s, assume Run2!", configFlags.IOVDb.DatabaseInstance)
            log.info("Using Run 2 configuration")

        folder  = "/TRIGGER/LUMI/HLTPrefLumi"
        result.merge (addFolders (configFlags, folder, 'TRIGGER_ONL',
                                  className = 'CondAttrListCollection'))

    kwargs['LuminosityFolderInputKey'] = folder
    log.info ("Created online %s using folder %s" % (name, folder))

    # Need the calibration just to get the proper MuToLumi value
    from CoolLumiUtilities.OnlineLumiCalibrationCondAlgConfig \
        import OnlineLumiCalibrationCondAlgCfg
    result.merge (OnlineLumiCalibrationCondAlgCfg(configFlags))
    olalg = result.getCondAlgo ('OnlineLumiCalibrationCondAlg')
    kwargs['OnlineLumiCalibrationInputKey'] = olalg.LumiCalibOutputKey
    
    # Other folder names should be blank.
    kwargs['BunchLumisInputKey'] = ''
    kwargs['BunchGroupInputKey'] = ''
    kwargs['FillParamsInputKey'] = ''

    return kwargs


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    print ('--- run2')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = LuminosityCondAlgCfg (flags1)
    acc1.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    acc1.wasMerged()

    print ('--- run2/express')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.RAW
    flags2.Common.doExpressProcessing = True
    flags2.lock()
    acc2 = LuminosityCondAlgCfg (flags2)
    acc2.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc2.getService('IOVDbSvc').Folders)
    acc2.wasMerged()

    print ('--- run1')
    flags3 = ConfigFlags.clone()
    flags3.Input.Files = defaultTestFiles.RAW
    flags3.Input.ProjectName = 'data12_8TeV'
    flags3.lock()
    acc3 = LuminosityCondAlgCfg (flags3)
    acc3.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc3.getService('IOVDbSvc').Folders)
    acc3.wasMerged()

    print ('--- run1/express')
    flags4 = ConfigFlags.clone()
    flags4.Input.Files = defaultTestFiles.RAW
    flags4.Input.ProjectName = 'data12_8TeV'
    flags4.Common.doExpressProcessing = True
    flags4.lock()
    acc4 = LuminosityCondAlgCfg (flags4)
    acc4.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc4.getService('IOVDbSvc').Folders)
    acc4.wasMerged()

    print ('--- mc')
    flags5 = ConfigFlags.clone()
    flags5.Input.Files = defaultTestFiles.ESD
    flags5.lock()
    acc5 = LuminosityCondAlgCfg (flags5)
    acc5.printCondAlgs(summariseProps=True)
    acc5.wasMerged()

    print ('--- online')
    flags6 = ConfigFlags.clone()
    flags6.Input.Files = defaultTestFiles.RAW
    flags6.Common.useOnlineLumi = True
    flags6.lock()
    acc6 = LuminosityCondAlgCfg (flags6)
    acc6.printCondAlgs(summariseProps=True)
    acc6.wasMerged()

    print ('--- forced online')
    flags7 = ConfigFlags.clone()
    flags7.Input.Files = defaultTestFiles.RAW
    flags7.lock()
    acc7 = LuminosityCondAlgCfg (flags7, useOnlineLumi=True, suffix='Online')
    acc7.printCondAlgs(summariseProps=True)
    acc7.wasMerged()
