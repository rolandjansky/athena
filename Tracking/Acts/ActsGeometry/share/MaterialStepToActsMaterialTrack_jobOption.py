# This job options creates database files of Geantino/MaterialMapping.
# Mofifled version of Tracking/TrkDetDescr/TrkDetDescrExample/share/MaterialMapping_jobOptions.py. (TrkDetDescrExample-01-00-04)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/UpgradeSimulationInnerTrackerMigrationRel20p3p3#Creation_of_database_files

# necessity to create a new PoolFileCatalog
import os
if os.path.exists('./PoolFileCatalog.xml') :
   print '[!] PoolFileCatalog exists in the run directory (may use old PFN!)'
   print '[>] Deleting it now !'
   os.remove('./PoolFileCatalog.xml')

###############################################################
#
# Job options
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["Acts DATAFILE='material-tracks.root' OPT='RECREATE'" ]


import glob
FileList = glob.glob("MaterialStepCollection*.root*")

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = FileList
if not hasattr(svcMgr, theApp.EventLoop):
     svcMgr += getattr(CfgMgr, theApp.EventLoop)()
evtloop = getattr(svcMgr, theApp.EventLoop)
try:
  evtloop.EventPrintoutInterval = 1000 
except Exception, err:
  msg.info('failed suppressing event loop heartbeat. performances might be sub-par... sorry.')
  pass

print "FileList = ", FileList

# Number of events to be processed
theApp.EvtMax = -1


# set up the Material Mapping
from ActsGeometry.ActsGeometryConf import MaterialStepToActsMaterialTrack
alg = MaterialStepToActsMaterialTrack(name ='MaterialStepToActsMaterialTrack')

job += alg
