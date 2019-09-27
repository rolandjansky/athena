if (not 'usePickleConfig' in dir()):
    usePickleConfig = False

if (not 'pickleconfigfile' in dir()):
    usePickleConfig = False

if (not 'DataSource' in dir()):
    DataSource = 'data'

if (not 'InputFormat' in dir()):
    InputFormat = 'bytestream'

if (not 'fileName' in dir()):
    fileName = None

if (not 'doESD' in dir()):
    doESD = True

if (not 'writeESD' in dir()):
    writeESD = False

if (not 'doAOD' in dir()):
    doAOD = False

if (not 'writeAOD' in dir()):
    writeAOD = False

if (not 'isOnlineStateless' in dir()):
    isOnlineStateless = True

if (not 'beamType' in dir()):
    beamType = 'collisions'

#if (not 'is_T0_project_tag' in dir()):
#    is_T0_project_tag = 'data12_8TeV'

if (not 'is_T0_project_tag' in dir()):
    is_T0_project_tag = 'data15_cos'

### tag created 2015-11-04 : Based on CONDBR2-HLTP-2015-07, TRT folders for rel. 20.7 were added and updated.
if (not 'ConditionsTag' in dir()):
    ConditionsTag = 'CONDBR2-HLTP-2015-08'

if (not 'DetDescrVersion' in dir()):
    DetDescrVersion = 'ATLAS-R2-2015-03-01-00'

if (not 'doPixelOnlyMon' in dir()):
    doPixelOnlyMon = False

if (not 'REO_doAutoConfiguration' in dir()):
    REO_doAutoConfiguration = False

### harmonize conditions tag and geometry for all monitoring jobs; 26 May 2017
DetDescrVersion = 'ATLAS-R2-2016-01-00-01' # for new ID alignment 26 May 2017
if isOnline:
    ConditionsTag = 'CONDBR2-HLTP-2017-03' # for new ID alignment 26 May 2017

## -------------------------------------------------------------------
## configuration from pickle file

import os,pickle

from AthenaCommon.Logging import logging
logRecExOnline_globalconfig = logging.getLogger( 'RecExOnline_globalconfig' )

if usePickleConfig and os.path.exists(pickleconfigfile):
    f = open(pickleconfigfile, 'r')
    onlinegoodies = pickle.load(f)
    f.close()

    if onlinegoodies.has_key('DataSource'):
        DataSource = onlinegoodies['DataSource']
    if onlinegoodies.has_key('ConditionsTag'):
        ConditionsTag = onlinegoodies['ConditionsTag']
    if onlinegoodies.has_key('beamType'):
        beamType = onlinegoodies['beamType']
    if onlinegoodies.has_key('DetDescrVersion'):
        DetDescrVersion = onlinegoodies['DetDescrVersion']

    if onlinegoodies.has_key('preExec'):
        preExec = onlinegoodies['preExec'].split(',,')
    if onlinegoodies.has_key('postExec'):
        postExec = onlinegoodies['postExec'].split(',,')
    if onlinegoodies.has_key('preInclude'):
        preInclude = onlinegoodies['preInclude'].split(',,')
    if onlinegoodies.has_key('postInclude'):
        postInclude = onlinegoodies['postInclude'].split(',,')

## -------------------------------------------------------------------
## Online Configuration from IS

import sys

# The dl module seems to be missing on 64-bit platforms.
# Since RTLD_NOW==0x002 and RTLD_GLOBAL==0x100 very commonly
# we will just guess that the proper flags are 0x102 when there
# is no dl module.
try:
    import dl
    newflags = dl.RTLD_NOW|dl.RTLD_GLOBAL
    logRecExOnline_globalconfig.info("import dl failed with newflags=%s" % newflags )
    logRecExOnline_globalconfig.info("proceeding with preconfigured newflags=0x102")
except:
    newflags = 0x102  # No dl module, so guess (see above).
try:
    sys.setdlopenflags(newflags)
except:
    logRecExOnline_globalconfig.info("sys.setdlopenflags(newflags) failed with newflags=%s" % newflags )
#import dl
#sys.setdlopenflags(dl.RTLD_GLOBAL | dl.RTLD_NOW)


### remember flags to set this correctly via default
from AthenaCommon.BFieldFlags import jobproperties

FirstSample = 3
NSamples = 5

### initialize flags before SetupField to choose correct condition tag database. 5 May 2017
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
globalflags.DataSource.set_Value_and_Lock(DataSource)
athenaCommonFlags.isOnline = isOnline

if (useEmon and (partitionName == 'ATLAS' or partitionName == 'ATLAS_MP1')):
    import ispy
    from ispy import *
    from ipc import IPCPartition
    from ispy import ISObject

    p2  = ispy.IPCPartition(partitionName)
    obj = ispy.ISObject(p2, 'RunParams.RunParams', 'RunParams')
    obj.checkout()
    is_T0_project_tag = obj.T0_project_tag
    is_run_number = obj.run_number
    is_beam_type = obj.beam_type
    logRecExOnline_globalconfig.info("is_run_number = %s" % is_run_number)
    logRecExOnline_globalconfig.info("is_beam_type = %s" % is_beam_type)
    logRecExOnline_globalconfig.info("is_T0_project_tag = %s" % is_T0_project_tag)

    part_name = 'initial'
    p = IPCPartition(part_name)
    toroidCurrent = 0
    toroidInvalid = 1
    solenoidCurrent = 0
    solenoidInvalid = 1

    # AL playing around: start
    #toroidCurrent   = ispy.ISInfoDynAny(p2, 'DdcFloatInfo')
    #solenoidCurrent = ispy.ISInfoDynAny(p2, 'DdcFloatInfo')
    #toroidInvalid   = ispy.ISInfoDynAny(p2, 'DdcIntInfo')
    #solenoidInvalid = ispy.ISInfoDynAny(p2, 'DdcIntInfo')
    solenoidCurrent = ISObject(p,'DCS_GENERAL.MagnetSolenoidCurrent.value','DdcFloatInfo')
    solenoidCurrent.checkout()
    toroidCurrent = ISObject(p,'DCS_GENERAL.MagnetToroidsCurrent.value','DdcFloatInfo')
    toroidCurrent.checkout()
    solenoidInvalid = ISObject(p,'DCS_GENERAL.MagnetSolenoidCurrent.invalid','DdcIntInfo')
    solenoidInvalid.checkout()
    toroidInvalid = ISObject(p,'DCS_GENERAL.MagnetToroidsCurrent.invalid','DdcIntInfo')
    toroidInvalid.checkout()

    #toroidInvalid.value=1
    #solenoidInvalid.value=1
    # AL playing around: end


    logRecExOnline_globalconfig.info("toroidCurrent = %f", toroidCurrent.value)
    logRecExOnline_globalconfig.info("toroidInvalid = %f", toroidInvalid.value)
    logRecExOnline_globalconfig.info("solenoidCurrent = %f", solenoidCurrent.value)
    logRecExOnline_globalconfig.info("solenoidInvalid = %f", solenoidInvalid.value)

    #from BFieldAth.BFieldAthConf import MagFieldAthenaSvc
    #svcMgr += MagFieldAthenaSvc(SetupCOOL=True, NameOfTheSource='COOL_HLT', onlineSolCur=solenoidCurrent.value, onlineTorCur=toroidCurrent.value)
    import MagFieldServices.SetupField # New magnetic field service. 5 May 2017

    try:
        p3 = IPCPartition(partitionName)
        x = ISObject(p3, 'LArParams.LAr.RunLogger.GlobalParams', 'GlobalParamsInfo')
        x.checkout()
        FirstSample = x.firstSample
        NSamples = x.nbOfSamples
        Format = x.format
    except:
        logRecExOnline_globalconfig.info("Could not find IS Parameters for LArParams.LAr.RunLogger.GlobalParams - Set default flags (FirstSample=3, NSamples=5")

    # for beam splash when LAr in "transparent" ROD mode  (5 May 2017)
    if 'Format' in dir() and Format == 0:
        from LArROD.LArRODFlags import larRODFlags
        larRODFlags.keepDSPRaw.set_Value_and_Lock(False)




# ----------------------------------------------- Set Run configuration

## Second, fall back to manual configuration or default values
globalflags.InputFormat.set_Value_and_Lock(InputFormat)

if not useEmon:
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock([fileName])
else:
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock([''])

## suppress needless warnings "no stream X associated with id Y" for online jobs
from GaudiSvc.GaudiSvcConf import THistSvc
THistSvc.OutputLevel = ERROR

### switch off NN Pixel cluster splitting, as suggested in bug #91340
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doPixelClusterSplitting.set_Value(False) # does not work online

## new ID alignment 26 May 2017
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags
GeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(True)

# ----------------------------------------------- Online flag
athenaCommonFlags.EvtMax.set_Value_and_Lock(evtMax)
athenaCommonFlags.isOnlineStateless = isOnlineStateless
# ----------------------------------------------- Remove DCS problems
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.useBeamConstraint.set_Value_and_Lock(False)

InDetFlags.doRobustReco = False

if athenaCommonFlags.isOnline:
    InDetFlags.useDCS.set_Value_and_Lock(False)                               # fails when true: pixel accessing Temperature info (getting fixed)

# ----------------------------------------------- Special settings for pixel monitoring
logRecExOnline_globalconfig.info("RecExOnline: flag doPixelOnlyMon = %s" %doPixelOnlyMon)
if doPixelOnlyMon:
    ## force pixel on, turn off the rest of the ID:
    InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock(True)
    InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock(False)
    InDetFlags.doTrtSegments.set_Value_and_Lock(False)

    from AthenaCommon.DetFlags import DetFlags
    DetFlags.SCT_setOff()
    DetFlags.detdescr.SCT_setOn()
    DetFlags.TRT_setOff()
    DetFlags.detdescr.TRT_setOn()
    DetFlags.pixel_setOn()
    DetFlags.detdescr.pixel_setOn()
    DetFlags.detdescr.ID_on()

# ----------------------------------------------- Output flags
from RecExConfig.RecFlags import rec

if (isOnline and useEmon and (partitionName == 'ATLAS' or partitionName == 'ATLAS_MP1' or partitionName == 'TDAQ')):
    rec.projectName.set_Value_and_Lock(is_T0_project_tag)
    from AthenaCommon.BeamFlags import jobproperties
    jobproperties.Beam.beamType.set_Value_and_Lock(beamType)
    globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)
    globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)
    rec.AutoConfiguration = ['FieldAndGeo', 'ConditionsTag', 'BeamType', 'BeamEnergy', 'LumiFlags']

# ------------------------------------------------ Ensure the current in tororid and solenoid
    from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
    jobproperties.BField.solenoidOn.set_Value_and_Lock( solenoidCurrent.value > 70 )
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock( toroidCurrent.value > 70 )
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock( toroidCurrent.value > 70 )

    #rec.AutoConfiguration = ['FieldAndGeo', 'ConditionsTag', 'BeamType', 'BeamEnergy', 'LumiFlags']
    #rec.AutoConfiguration = ['ConditionsTag', 'BeamType', 'BeamEnergy', 'LumiFlags']
    logRecExOnline_globalconfig.info("RecExOnline: running with autoconfiguration ['FieldAndGeo', 'ConditionsTag', 'BeamType', 'BeamEnergy', 'LumiFlags']")
    #logRecExOnline_globalconfig.info("RecExOnline: FieldAndGeo = %s" %FieldAndGeo)
    #logRecExOnline_globalconfig.info("RecExOnline: ConditionsTag = %s" %ConditionsTag)
    #logRecExOnline_globalconfig.info("RecExOnline: BeamType = %s" %BeamType)
    #logRecExOnline_globalconfig.info("RecExOnline: BeamEnergy = %s" %BeamEnergy)
    #logRecExOnline_globalconfig.info("RecExOnline: LumiFlags = %s" %LumiFlags)

if (isOnline and useEmon and (partitionName != 'ATLAS' and partitionName != 'ATLAS_MP1' and partitionName != 'TDAQ')):
    from AthenaCommon.BeamFlags import jobproperties
    jobproperties.Beam.beamType.set_Value_and_Lock(beamType)
    globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)
    globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)
    rec.AutoConfiguration = ['FieldAndGeo', 'ConditionsTag', 'BeamType']
#    rec.AutoConfiguration = ['ConditionsTag', 'BeamType']
    #logRecExOnline_globalconfig.info("RecExOnline: running with autoconfiguration ['FieldAndGeo', 'ConditionsTag', 'BeamType']")
    #logRecExOnline_globalconfig.info("RecExOnline: FieldAndGeo = %s" %FieldAndGeo)
    #logRecExOnline_globalconfig.info("RecExOnline: ConditionsTag = %s" %ConditionsTag)
    #logRecExOnline_globalconfig.info("RecExOnline: BeamType = %s" %BeamType)
if (not isOnline and not useEmon) or (isOnline and REO_doAutoConfiguration):
    logRecExOnline_globalconfig.info("RecExOnline: (not isOnline and not useEmon) or (isOnline and REO_doAutoConfiguration)")
    rec.AutoConfiguration = ['everything']
    logRecExOnline_globalconfig.info("RecExOnline: running with autoconfiguration = everything")


if (isOnline and not useEmon):
    logRecExOnline_globalconfig.info("RecExOnline: isOnline and not useEmon")
    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
    from AthenaCommon.BeamFlags import jobproperties
    jobproperties.Beam.beamType.set_Value_and_Lock(beamType)
    globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)
    globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)
    ### rec.AutoConfiguration = ['FieldAndGeo', 'ConditionsTag', 'BeamType', 'BeamEnergy', 'LumiFlags']
    rec.AutoConfiguration = ['FieldAndGeo', 'ConditionsTag', 'BeamType']
    #    rec.AutoConfiguration = ['ConditionsTag', 'BeamType']
    logRecExOnline_globalconfig.info("RecExOnline: running with autoconfiguration ['FieldAndGeo', 'ConditionsTag', 'BeamType']")
    # logRecExOnline_globalconfig.info("RecExOnline: FieldAndGeo = %s" %FieldAndGeo)
    # logRecExOnline_globalconfig.info("RecExOnline: ConditionsTag = %s" %ConditionsTag)
    # logRecExOnline_globalconfig.info("RecExOnline: BeamType = %s" %BeamType)



#### CHANGES TO GET 19.1.0.1 RECO WORKING (M4)
# Default to COMP200 rather than CONDBR2
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DatabaseInstance = "CONDBR2"
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

## Another Problem with LAR offline databases
from LArConditionsCommon.LArCondFlags import larCondFlags
#larCondFlags.SingleVersion.set_Value_and_Lock(True)
larCondFlags.OFCShapeFolder.set_Value_and_Lock("")
larCondFlags.ua2MeVFolder.set_Value_and_Lock("")
larCondFlags.MphysOverMcalFolder.set_Value_and_Lock("")


rec.doESD.set_Value_and_Lock(doESD)
rec.doAOD.set_Value_and_Lock(doAOD)
rec.doWriteESD.set_Value_and_Lock(writeESD)
rec.doWriteAOD.set_Value_and_Lock(writeAOD)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)

# Fixing the error message for deprecated doAOD call: "ERROR updating doAOD to value False . WILL SOON NOT WORK ANYMORE ! Please update to filling directly jp.Rec.blah or jp.AthenaCommonFlags.blah "
#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.doAOD.set_Value_and_Lock(doAOD)
#
#from RecExConfig.RecFlags import jobproperties
#jobproperties.Rec.doAOD.set_Value_and_Lock(doAOD)



# ---------------------------------------------- Debug flags
rec.doPerfMon.set_Value_and_Lock(False)                                     # optional for performance check
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)
rec.doNameAuditor.set_Value_and_Lock(False)                                 # optional for debugging
rec.doDetStatus.set_Value_and_Lock(False)                                   # fails when true (fixed in next release)

if not 'IOVDbSvcMessage' in dir():
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    import IOVDbSvc.IOVDb
    svcMgr.IOVDbSvc.OutputLevel = 2

