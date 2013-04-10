## @file EventCheck.py
## @brief TopOptions: Using command line argument In, read 1 event from In
## and check that all the objects in the DataHeader are readable.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: CopyFiles.py,v 1.17 2008-08-05 16:32:26 schaffer Exp $
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
#import glob, os, re
import traceback

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import theApp

try:
  theApp.EvtMax = EvtMax
except:
  theApp.EvtMax = 200000
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()

svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]

import AthenaPoolCnvSvc.ReadAthenaPool

# get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

from CLIDComps.CLIDCompsConf import ClassIDSvc
svcMgr += ClassIDSvc()
include( "PartPropSvc/PartPropSvc.py" )

# load all possible converters for EventCheck
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.IgnoreTagDifference=True

# set up all detector description stuff 
include( "RecExCond/AllDet_detDescr.py" )

# Take the file name form the command line -c options
EventSelector = svcMgr.EventSelector
try:
  EventSelector.InputCollections = In
except:
  print "Usage: -c 'In=['file']'"
# Check collection type
try:
  EventSelector.CollectionType = CollType
except:
  print "Reading from file"

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
try: 
  Stream1 = AthenaPoolOutputStream( "Stream1", Out, True )
except:
  Stream1 = AthenaPoolOutputStream( "Stream1", "DidNotSetOutputName.root", True )
Stream1.TakeItemsFromInput=TRUE;
Stream1.ForceRead=TRUE;  #force read of output data objs
# The next line is an example on how to exclude clid's if they are causing a  problem
#Stream1.ExcludeList = ['6421#*']

# Look for lists of filter algorithms
try:
  Stream1.AcceptAlgs = AcceptList
except:
  print "No accept algs indicated in AcceptList"
try:
  Stream1.RequireAlgs = RequireList
except:
  print "No accept algs indicated in RequireList"
try:
  Stream1.VetoAlgs = VetoList
except:
  print "No accept algs indicated in VetoList"

from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
svcMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]

# EventCheck configuration ------------------------ END

MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = INFO
#==============================================================
#
# End of job options file
#
###############################################################
