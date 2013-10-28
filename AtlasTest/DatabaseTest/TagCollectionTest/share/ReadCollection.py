## @file ReadCollection.py
## @brief Reads back event tag metadata from a collection
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: ReadCollection.py,v 1.9 2008-08-04 12:55:42 cranshaw Exp $

###############################################################
#
# Job options file
#
#==============================================================
# Input: 1) A Collection called "InputCollection", by default,
#           containing references to persistified event data
#           and associated event tag metadata.
# Output: None
#==============================================================

#--------------------------------------------------------------
# Message service output level threshold
# (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Number of events to read back
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp

try:
  theApp.EvtMax = EvtMax
except:
  theApp.EvtMax = 20

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Message service printout limits
#--------------------------------------------------------------
try:
  svcMgr.MessageSvc.OutputLevel = MsgLevel
except:
  svcMgr.MessageSvc.OutputLevel = INFO

svcMgr.MessageSvc.debugLimit = 5000
#--------------------------------------------------------------
# POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
try:
  PoolSvc.ReadCatalog = InCatalog
except:
  PoolSvc.ReadCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]

#--------------------------------------------------------------
# Event selector
#--------------------------------------------------------------
# List of input collections:
try:
  svcMgr.EventSelector.InputCollections = InColl
except:
  svcMgr.EventSelector.InputCollections = [ "InputCollection" ]

# Type of input collections:
try:
  svcMgr.EventSelector.CollectionType = InCollType
except:
  svcMgr.EventSelector.CollectionType = "ExplicitROOT"

# Connection string to database containing the input collections:
try:
  svcMgr.EventSelector.Connection = InCollConnect
except:
  svcMgr.EventSelector.Connection = ""

# Query applied to event tag collection metadata:
try:
  svcMgr.EventSelector.Query = Query
except:
  svcMgr.EventSelector.Query = ""

try: 
  svcMgr.EventSelector.RefName = InputRef
except:
  print "Taking primary token as input"

#--------------------------------------------------------------
# Application: Read back dummy event tag collection metadata
#--------------------------------------------------------------
from AthenaPoolMultiTest.AthenaPoolMultiTestConf import EventTagReader

EventTagReader = EventTagReader( "EventTagReader" )

topSequence+=EventTagReader

EventTagReader.OutputLevel = DEBUG

#==============================================================
#
# End of job options file
#
###############################################################
