if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-20-00-01"
    print "MuonGeoModelTest/simulGeantinoHits Setting now DetDescrVersion to ", DetDescrVersion
else:
    print "MuonGeoModelTest/simulGeantinoHits DetDescrVersion already set to ", DetDescrVersion

# now it's necessary to specify the global condition tag
if not 'ConditionsTag' in dir():
   ConditionsTag="OFLCOND-MC12b-SIM-00"
   print "MuonGeoModelTest/simulGeantinoHits Setting now ConditionsTag to ", ConditionsTag
else:
   print "MuonGeoModelTest/simulGeantinoHits ConditionsTag already set to ", ConditionsTag

#--- Algorithm sequence ---------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.PoolHitsOutput='geantinoHits.pool.root'
athenaCommonFlags.EvtMax=10000
athenaCommonFlags.SkipEvents=0

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.SimLayout.set_On()
SimFlags.SimLayout=DetDescrVersion # specific value 
SimFlags.load_atlas_flags()
#  sets the EtaPhi, VertexSpread and VertexRange checks on
#SimFlags.SimLayout.set_On()            # default value
SimFlags.EventFilter.set_Off()  

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.ConditionsTag = ConditionsTag
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00"

#--- ParticleGun ----------------------------------------------
import ParticleGun as PG
pg = PG.ParticleGun()
#pg.sampler.pid = (13, -13)
pg.sampler.pid = 999
#pg.sampler.mom = PG.PtEtaMPhiSampler(pt=1000000, eta=[-1.1,1.1], phi=[-PG.PI, PG.PI])
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=100000, eta=[-1.1,1.1], phi=[3.49,5.934])
topSeq += pg

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================
from G4AtlasApps import SimKernel
