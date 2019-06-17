# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: LumiBlockComps/python/LuminosityCondAlgDefault.py
# Created: May 2019, sss, from existing LuminosityToolDefault.
# Purpose: Configure LuminosityCondAlg.
#

from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import AthSequencer


def configureLuminosityCondAlgMC (name):
    return { 'LuminosityFolderInputKey' : '',
             'OnlineLumiCalibrationInputKey' : '',
             'BunchLumisInputKey' : '',
             'BunchGroupInputKey' : '',
             'FillParamsInputKey' : '' }
             


# Configuration for offline default tool used in Run2
def configureLuminosityCondAlgRun2 (name):
    mlog = logging.getLogger(name)

    kwargs = {}

    # Set up DB configuration
    from IOVDbSvc.CondDB import conddb
    from RecExConfig.RecFlags import rec

    # Check if this is express stream or bulk
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if rec.doExpressProcessing() or athenaCommonFlags.isOnline:
        lumiFolder  = "/TRIGGER/LUMI/OnlPrefLumi"
        conddb.addFolder('TRIGGER_ONL', lumiFolder,
                         className = 'CondAttrListCollection')

    else:
        lumiFolder = "/TRIGGER/OFLLUMI/OflPrefLumi"
        conddb.addFolder('TRIGGER_OFL', lumiFolder,
                         className = 'CondAttrListCollection')

    mlog.info("configureLuminosityCondAlgRun2 requested %s", lumiFolder)
    kwargs['LuminosityFolderInputKey'] = lumiFolder

    mlog.info("Created Run2 %s using folder %s" % (name, lumiFolder))

    # Need the calibration tool just to get the proper MuToLumi value
    from CoolLumiUtilities.OnlineLumiCalibrationCondAlgDefault \
        import OnlineLumiCalibrationCondAlgDefault
    calibAlg = OnlineLumiCalibrationCondAlgDefault()
    kwargs['OnlineLumiCalibrationInputKey'] = calibAlg.LumiCalibOutputKey
    
    # Other folder names should be blank.
    kwargs['BunchLumisInputKey'] = ''
    kwargs['BunchGroupInputKey'] = ''
    kwargs['FillParamsInputKey'] = ''

    return kwargs


# Configuration for offline default tool used in Run1
def configureLuminosityCondAlgRun1 (name):
    mlog = logging.getLogger(name)

    kwargs = {}
       
    # Now configure DB based on the environment
    from IOVDbSvc.CondDB import conddb
    from RecExConfig.RecFlags import rec

    # Check if this is express stream or bulk
    if rec.doExpressProcessing():
        lumiFolder  = "/TRIGGER/LUMI/LBLESTONL"
        conddb.addFolder('TRIGGER_ONL', lumiFolder,
                         className = 'CondAttrListCollection')
        mlog.info("configureLuminosityCondAlgRun1 requested %s", lumiFolder)

    else:
        lumiFolder = "/TRIGGER/OFLLUMI/LBLESTOFL"
        conddb.addFolder('TRIGGER_OFL', lumiFolder,
                         className = 'CondAttrListCollection')
        mlog.info("configureLuminosityCondAlgRun1 requested %s", lumiFolder)

    kwargs['LuminosityFolderInputKey'] = lumiFolder

    # Configure input conditions data.
    from CoolLumiUtilities.FillParamsCondAlgDefault \
        import FillParamsCondAlgDefault
    fillParamsAlg = FillParamsCondAlgDefault()
    kwargs['FillParamsInputKey'] = fillParamsAlg.FillParamsOutputKey

    from CoolLumiUtilities.BunchLumisCondAlgDefault \
        import BunchLumisCondAlgDefault
    bunchLumisAlg = BunchLumisCondAlgDefault()
    kwargs['BunchLumisInputKey'] = bunchLumisAlg.BunchLumisOutputKey

    from CoolLumiUtilities.BunchGroupCondAlgDefault \
        import BunchGroupCondAlgDefault
    bunchGroupAlg = BunchGroupCondAlgDefault()
    kwargs['BunchGroupInputKey'] = bunchGroupAlg.BunchGroupOutputKey

    from CoolLumiUtilities.OnlineLumiCalibrationCondAlgDefault \
        import OnlineLumiCalibrationCondAlgDefault
    calibAlg = OnlineLumiCalibrationCondAlgDefault()
    kwargs['OnlineLumiCalibrationInputKey'] = calibAlg.LumiCalibOutputKey

    return kwargs


def LuminosityCondAlgDefault (name = 'LuminosityCondAlg', kwargs = None):
    mlog = logging.getLogger(name)
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from IOVDbSvc.CondDB import conddb
    if kwargs != None:
        pass

    elif conddb.isMC:
         mlog.info("LuminosityCondAlgDefault called for MC!")
         kwargs = configureLuminosityCondAlgMC (name)

    elif conddb.dbdata == "COMP200":
        kwargs = configureLuminosityCondAlgRun1 (name)

    elif conddb.dbdata == "CONDBR2":
        kwargs = configureLuminosityCondAlgRun2 (name)

    else:
        mlog.warning("LuminosityToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
        kwargs = configureLuminosityCondAlgRun2 (name)
    
    from LumiBlockComps.LumiBlockCompsConf import \
        LuminosityCondAlg

    alg = LuminosityCondAlg (name,
                             LuminosityOutputKey = 'LuminosityCondData',
                             **kwargs)
    condSeq += alg

    return alg


def LuminosityCondAlgOnline (name = 'LuminosityCondAlg'):
    mlog = logging.getLogger(name)

    kwargs = {}

    # Keep values for invalid data
    kwargs['SkipInvalid'] = False
        
    from IOVDbSvc.CondDB import conddb
    if conddb.dbdata == "COMP200": # Run1
        folder  = "/TRIGGER/LUMI/LBLESTONL"
        conddb.addFolder('TRIGGER_ONL', folder,
                         className = 'CondAttrListCollection')
      
    else: #  Run 2
        if conddb.dbdata != "CONDBR2":
            mlog.warning("LuminosityToolOnline can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
            mlog.info("Using Run 2 configuration")

        folder  = "/TRIGGER/LUMI/HLTPrefLumi"
        conddb.addFolder('TRIGGER_ONL', folder,
                         className = 'CondAttrListCollection')

    kwargs['LuminosityFolderInputKey'] = folder
    mlog.info("Created online %s using folder %s" % (name, folder))

    # Need the calibration tool just to get the proper MuToLumi value
    from CoolLumiUtilities.OnlineLumiCalibrationCondAlgDefault \
        import OnlineLumiCalibrationCondAlgDefault
    calibAlg = OnlineLumiCalibrationCondAlgDefault()
    kwargs['OnlineLumiCalibrationInputKey'] = calibAlg.LumiCalibOutputKey
    
    # Other folder names should be blank.
    kwargs['BunchLumisInputKey'] = ''
    kwargs['BunchGroupInputKey'] = ''
    kwargs['FillParamsInputKey'] = ''

    return kwargs
