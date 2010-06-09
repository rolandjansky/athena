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


#old: from AthenaCommon.GlobalFlags import GlobalFlags
#old: GlobalFlags.DetGeo.set_atlas()
#old: GlobalFlags.DataSource.set_geant4()
#old: GlobalFlags.InputFormat.set_pool()
#old: GlobalFlags.Luminosity.set_zero()
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
svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/a/andr/scratch0/mc08.105200.T1_McAtNlo_Jimmy.recon.AOD.e357_s462_r541/AOD.025888._00001.pool.root.1" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit  = 100000

# Number of Events to process
theApp.EvtMax = 4

#================================================================
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from TopEDM_Test.TopEDM_TestConf import *
job += TopEDM_WriteTest()

job.TopEDM_WriteTest.OutputLevel = DEBUG

sg = Service("StoreGateSvc")
sg.Dump = True

#================================================================
from OutputStreamAthenaPool.OutputStreamAthenaPool import AthenaPoolOutputStream
ostr = AthenaPoolOutputStream( "TTbarStream", "ttbar.pool.root" )

# need to write out collections for which we store ELs
ostr.ItemList += [ "JetCollection#Cone4H1TowerJets",
                   "ElectronContainer#ElectronAODCollection",
                   "MissingET#MET_RefFinal",
                   "Analysis::MuonContainer#StacoMuonCollection"
                   ]

# our ttbar classes
ostr.ItemList += [
    "TopRec::TTbarJJContainer#*",
    "TopRec::TTbarJJTopContainer#*",

    "TopRec::TTbarEJContainer#*",
    "TopRec::TTbarEJMetContainer#*",
    "TopRec::TTbarEJTopContainer#*",
    "TopRec::TTbarEJDiTopsContainer#*",

    "TopRec::TTbarEEContainer#*",
    "TopRec::TTbarEEJetsContainer#*",
    "TopRec::TTbarEEJetsMetContainer#*",

    "TopRec::TTbarMuJContainer#*",
    ]

#================================================================
ServiceMgr.MessageSvc.Format       = "% F%40W%S%7W%R%T %0W%M"

#================================================================
print "Printing out complete (?) job configuration"

def strendl(a):
    return str(a) + "\n"

from AthenaCommon.ConfigurationShelve import ConfigurationJar
sys.stdout.writelines( map( strendl, ConfigurationJar( "MyConfigDump" ).content().values() ) )

#================================================================
