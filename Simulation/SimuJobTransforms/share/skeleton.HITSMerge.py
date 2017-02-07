#import glob, os, re
import traceback

from AthenaCommon.Logging import logging
merHitLog = logging.getLogger('MergeHITS')

merHitLog.info( '****************** STARTING HIT MERGING *****************' )

merHitLog.info( '**** Transformation run arguments' )
merHitLog.info( str(runArgs) )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#==============================================================
# Job definition parameters:
#==============================================================
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#Jobs should stop if an include fails.
if hasattr(runArgs,"IgnoreConfigError"):
    athenaCommonFlags.AllowIgnoreConfigError=runArgs.IgnoreConfigError
else:
    athenaCommonFlags.AllowIgnoreConfigError=False

from AthenaCommon.AppMgr import theApp
EvtMax=-1
if hasattr(runArgs,"maxEvents"):
    EvtMax = runArgs.maxEvents
theApp.EvtMax = EvtMax

#--------------------------------------------------------------
# Peek at input to configure DetFlags
#--------------------------------------------------------------
if not hasattr(runArgs,"inputHITSFile"):
    raise RuntimeError("No inputHITSFile provided.")

from SimuJobTransforms.HitsFilePeeker import HitsFilePeeker
HitsFilePeeker(runArgs, merHitLog)
from AthenaCommon.DetFlags import DetFlags
DetFlags.geometry.all_setOff()

#==============================================================
# Job Configuration parameters:
#==============================================================
## Pre-exec
if hasattr(runArgs,"preExec"):
    merHitLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        merHitLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
ServiceMgr += AthenaPoolCnvSvc()

from PyJobTransforms.trfUtils import releaseIsOlderThan
if releaseIsOlderThan(17,6):
    ServiceMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]

import AthenaPoolCnvSvc.ReadAthenaPool

from CLIDComps.CLIDCompsConf import ClassIDSvc
ServiceMgr += ClassIDSvc()
include( "PartPropSvc/PartPropSvc.py" )

# load all possible converters for EventCheck
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.IgnoreTagDifference=True

# set up all detector description stuff
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.DetFlags import DetFlags
DetFlags.Print()

#--------------------------------------------------------------
# Setup Input
#--------------------------------------------------------------
In = runArgs.inputHITSFile
EventSelector = ServiceMgr.EventSelector
EventSelector.InputCollections = In

# Check collection type
try:
  EventSelector.CollectionType = CollType
except:
  print "Reading from file"

SkipEvents=0
if hasattr(runArgs,"skipEvents"):
    SkipEvents = runArgs.skipEvents
ServiceMgr.EventSelector.SkipEvents = SkipEvents

#--------------------------------------------------------------
# Setup Output
#--------------------------------------------------------------
if not hasattr(runArgs,"outputHITS_MRGFile"):
    raise RuntimeError("No outputHITS_MRGFile provided.")
Out = runArgs.outputHITS_MRGFile
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
try:
  StreamHITS = AthenaPoolOutputStream( "StreamHITS", Out, True )
except:
  StreamHITS = AthenaPoolOutputStream( "StreamHITS", "DidNotSetOutputName.root", True )
StreamHITS.TakeItemsFromInput=TRUE;
StreamHITS.ForceRead=TRUE;  #force read of output data objs
# The next line is an example on how to exclude clid's if they are causing a  problem
#StreamHITS.ExcludeList = ['6421#*']

# Look for lists of filter algorithms
try:
  StreamHITS.AcceptAlgs = AcceptList
except:
  print "No accept algs indicated in AcceptList"
try:
  StreamHITS.RequireAlgs = RequireList
except:
  print "No accept algs indicated in RequireList"
try:
  StreamHITS.VetoAlgs = VetoList
except:
  print "No accept algs indicated in VetoList"

# Perfmon
from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring=True
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True
pmon_properties.PerfMonFlags.OutputFile = "ntuple_HITSMerge"

MessageSvc = ServiceMgr.MessageSvc
MessageSvc.OutputLevel = INFO

StreamHITS.ExtendProvenanceRecord = False

ServiceMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "15000000000" ]

#--------------------------------------------------------------
# Ensure IOVDbSvc.GlobalTag is configured
# - protection against bad HITS file metadata
#--------------------------------------------------------------
if not hasattr(ServiceMgr,'IOVDbSvc'):
    from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
    ServiceMgr += IOVDbSvc()
if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
    from AthenaCommon.GlobalFlags import globalflags
    ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

#--------------------------------------------------------------

## Post-include
if hasattr(runArgs,"postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    merHitLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        merHitLog.info(cmd)
        exec(cmd)
#--------------------------------------------------------------
print topSequence
