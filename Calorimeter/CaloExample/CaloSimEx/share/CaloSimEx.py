#==============================================================
#
# Job options file for Geant4 Simulations
#
# Atlas simulation 
#
#==============================================================

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
#
DetFlags.Truth_setOn()


#--- Global conditions tag ------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS7T-05-07"

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolHitsOutput='atlas_MyOutputFile.root'
athenaCommonFlags.PoolHitsOutput='atlas_MyOutputFile.pool.root'
athenaCommonFlags.EvtMax =1
athenaCommonFlags.PoolEvgenInput.set_Off()

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
#bugs 73445 Comment #10
simFlags.load_atlas_flags()
## Use the default value:
simFlags.SimLayout.set_On()
## Set a specific layout tag:
#simFlags.SimLayout='ATLAS-GEO-16-00-00' # specific value 
simFlags.EventFilter.set_On()

#--- Use single particle gun ----------------------------
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 11
pg.sampler.mom = PG.EEtaMPhiSampler(energy=5000, eta=[-3,3])
topSeq += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3


#######################################################################

#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

