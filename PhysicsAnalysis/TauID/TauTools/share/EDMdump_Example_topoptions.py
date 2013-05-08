## @file TauTools/share/TauEDMdump_joboptions.py
## @purpose steering job option file for MyAlg residing in python/TauEDMdump.py
## @author Dugan O'Neil

## import the SI units
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Event selector
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# The AOD input file
ServiceMgr.EventSelector.InputCollections = ['/cluster/data01/endw/AOD/005188tid006904_calib/AOD.pool_0_0.root','/cluster/data01/endw/AOD/005188tid006904_calib/AOD.pool_1_0.root','/cluster/data01/endw/AOD/005188tid006904_calib/AOD.pool_2_0.root','/cluster/data01/endw/AOD/005188tid006904_calib/AOD.pool_3_0.root','/cluster/data01/endw/AOD/005188tid006904_calib/AOD.pool_4_0.root']
## get a handle on the application manager
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10

include ("TauTools/TauEDMdump_joboptions.py")
include ("TauTools/EDMdump_Example_joboptions.py")

##########################################
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc = Service( "MessageSvc" )
svcMgr.MessageSvc.OutputLevel = ERROR       
