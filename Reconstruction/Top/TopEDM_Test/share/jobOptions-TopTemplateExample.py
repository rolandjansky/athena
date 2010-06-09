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
import AthenaPoolCnvSvc.WriteAthenaPool


from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetGeo = 'atlas'
jobproperties.Global.DataSource = 'geant4'
jobproperties.Global.InputFormat = 'pool'
jobproperties.Global.Luminosity = 'zero'

#
from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOn()

# Geometry, controlled by DetFlags
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-02-01-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#================================================================
svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/a/andr/scratch0/valid1.005200.T1_McAtNlo_Jimmy.recon.AOD.e322_s483_r562_tid027403-AOD.027403._00001.20evts.pool.root" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000

# Number of Events to process
theApp.EvtMax = 4

#================================================================
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from TopEDM_Test.TopEDM_TestConf import *

# TTbar producer algorithm
job += TopEDM_WriteTest()
job.TopEDM_WriteTest.OutputLevel = DEBUG

# "analysis" on ttbar containers put in SG by the previous alg
job += TopTemplateExample()
job.TopTemplateExample.OutputLevel = DEBUG

#================================================================
ServiceMgr.MessageSvc.Format       = "% F%40W%S%7W%R%T %0W%M"

#================================================================
print "Printing out complete (?) job configuration"

def strendl(a):
    return str(a) + "\n"

from AthenaCommon.ConfigurationShelve import ConfigurationJar
sys.stdout.writelines( map( strendl, ConfigurationJar( "MyConfigDump" ).content().values() ) )

#================================================================
