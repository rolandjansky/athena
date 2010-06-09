## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetGeo = 'atlas'
jobproperties.Global.DataSource = 'geant4'
jobproperties.Global.InputFormat = 'pool'
jobproperties.Global.Luminosity = 'zero'

# AG:
#from IOVDbSvc.CondDB import conddb
#conddb.GlobalTag = 'COMCOND-004-01'

#
from AthenaCommon.DetFlags import DetFlags
#DetFlags.ID_setOn()
DetFlags.all_setOn()


# Geometry, controlled by DetFlags
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-02-01-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#================================================================

svcMgr.EventSelector.InputCollections = [ "ttbar.pool.root" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000

# Number of Events to process
theApp.EvtMax = 4

#================================================================
# AG:

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#from edmtest.edmtestConf import edmtestConf
from TopEDM_Test.TopEDM_TestConf import *
job += TopEDM_ReadTest()

sg = Service("StoreGateSvc")
sg.Dump = True

#================================================================
ServiceMgr.MessageSvc.Format       = "% F%40W%S%7W%R%T %0W%M"

#================================================================
print "Printing out complete (?) job configuration"

def strendl(a):
    return str(a) + "\n"

from AthenaCommon.ConfigurationShelve import ConfigurationJar
sys.stdout.writelines( map( strendl, ConfigurationJar( "MyConfigDump" ).content().values() ) )


#================================================================
