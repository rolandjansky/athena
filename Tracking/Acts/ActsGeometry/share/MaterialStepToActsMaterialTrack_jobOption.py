# This job options convert MaterialStepCollection to Material tracks to be used in the Acts MaterialMapping.

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
svcMgr.THistSvc.Output += ["Acts DATAFILE='material-tracks_4.root' OPT='RECREATE'" ]
svcMgr.MessageSvc.defaultLimit = 9999999

# Input file containing the MaterialStepCollection.
import glob
#FileList = glob.glob("MaterialStepCollection*.root*")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/*")

#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0000*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0001*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0002*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0003*.pool.root")
FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0004*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0005*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0006*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0007*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0008*.pool.root")
#FileList = glob.glob("/afs/cern.ch/work/t/tstreble/public/forNoemi/Step3p1_Geantino/user.tstreble.GeantinoMap_22_00_00_MaterialStream/user.tstreble.19401213.MaterialStream._0009*.pool.root")

# Event loop
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

# Number of events to be processed (-1 = all)
theApp.EvtMax = -1


# set up the MaterialStepToActsMaterialTrack
from ActsGeometry.ActsGeometryConf import MaterialStepToActsMaterialTrack
alg = MaterialStepToActsMaterialTrack(name ='MaterialStepToActsMaterialTrack')
job += alg
