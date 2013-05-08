## @file TauTools/share/TauEDMdump_topoptions.py
## @purpose steering job option file for MyAlg residing in python/TauEDMdump.py
## @author Dugan O'Neil

## import the SI units
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Event selector
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# The AOD input file
#ServiceMgr.EventSelector.InputCollections = ['/cluster/data01/endw/datasets/user08.NoelDawe_.calib.signal_005188tid006904/AOD.pool_0_0.root']
ServiceMgr.EventSelector.InputCollections = ['AOD.pool.root']
## get a handle on the application manager
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 5

##include ("TauTools/TauEDMdump_joboptions.py")
include ("TauEDMdump_joboptions.py")

##########################################
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc = Service( "MessageSvc" )
svcMgr.MessageSvc.OutputLevel = ERROR       
