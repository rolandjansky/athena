## @file EventCount.py
## @brief TopOptions: Using command line argument In, read all events from
## In to count them and find their run numbers. If dump flag is set, then an
## object count is also given.
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventCount.py,v 1.5 2007-10-18 13:46:20 gemmeren Exp $
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
#import glob, os, re

theApp.EvtMax = 200000
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
# Dummy flags to appease the god of JO
#useNova=False; MuonLayout='P03-DC2'; MuonDDUseNova=False;

import AthenaPoolCnvSvc.ReadAthenaPool

# get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import theApp

try:
  theApp.EvtMax = EvtMax
except:
  theApp.EvtMax = -1

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

try:
  EventSelector.Query = Query
except:
  pass

include ("AthenaPoolTools/MetadataTest_jobOptions.py")
                                                                               
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = INFO
#==============================================================
#
# End of job options file
#
###############################################################
