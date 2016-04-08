###############################################################
#
# Skeleton top job options for RDO merging
#
#==============================================================

#import glob, os, re
import traceback

from AthenaCommon.Logging import logging
merHitLog = logging.getLogger('MergeRDOS')

merHitLog.info( '****************** STARTING RDO MERGING *****************' )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#==============================================================
# Job definition parameters:
#==============================================================
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#Jobs should stop if an include fails.
athenaCommonFlags.AllowIgnoreConfigError.set_Value_and_Lock(False)
if hasattr(runArgs,"inputRDOFile"): athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputRDOFile )

#from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = -1 

from AthenaCommon.GlobalFlags import globalflags
if hasattr(runArgs,"geometryVersion"): globalflags.DetDescrVersion.set_Value_and_Lock( runArgs.geometryVersion )
if hasattr(runArgs,"conditionsTag"): globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )

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

ServiceMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]

import AthenaPoolCnvSvc.ReadAthenaPool

from CLIDComps.CLIDCompsConf import ClassIDSvc
ServiceMgr += ClassIDSvc()
include( "PartPropSvc/PartPropSvc.py" )

# load all possible converters for EventCheck
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.IgnoreTagDifference=True

# set up all detector description stuff 
include( "RecExCond/AllDet_detDescr.py" )

## TGCcablingServerSvc Hack
import MuonCnvExample.MuonCablingConfig

#--------------------------------------------------------------
# Setup Input
#--------------------------------------------------------------
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

# Check collection type
try:
  ServiceMgr.EventSelector.CollectionType = CollType
except:
  print "Reading from file"

SkipEvents=0
ServiceMgr.EventSelector.SkipEvents = SkipEvents

#--------------------------------------------------------------
# Setup Output
#--------------------------------------------------------------
if hasattr(runArgs,"outputRDOFile"): Out = runArgs.outputRDOFile

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
try: 
  StreamRDO = AthenaPoolOutputStream( "StreamRDO", Out, True )
except:
  StreamRDO = AthenaPoolOutputStream( "StreamRDO", "DidNotSetOutputName.root", True )
StreamRDO.TakeItemsFromInput=TRUE;
StreamRDO.ForceRead=TRUE;  #force read of output data objs
# The next line is an example on how to exclude clid's if they are causing a  problem
#StreamRDO.ExcludeList = ['6421#*']

# Look for lists of filter algorithms
try:
  StreamRDO.AcceptAlgs = AcceptList
except:
  print "No accept algs indicated in AcceptList"
try:
  StreamRDO.RequireAlgs = RequireList
except:
  print "No accept algs indicated in RequireList"
try:
  StreamRDO.VetoAlgs = VetoList
except:
  print "No accept algs indicated in VetoList"


MessageSvc = ServiceMgr.MessageSvc
MessageSvc.OutputLevel = INFO

StreamRDO.ExtendProvenanceRecord = False

ServiceMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "15000000000" ]

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
