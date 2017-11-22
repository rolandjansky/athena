#==============================================================
#
# Job options file for Geant4 Simulations
#
#  CTB simulation with 20 GeV photons  ID + Calorimeter in the setup
#
__version__="$Revision: 1.1 $"
#==============================================================

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.LAr_setOn()
DetFlags.em_setOn()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff() # muons are not supported in >=13.0.0
# - MCTruth
DetFlags.Truth_setOn()

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput='ctb_MyOutputFile.pool.root'
athenaCommonFlags.EvtMax=10
athenaCommonFlags.PoolEvgenInput.set_Off()

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
simFlags.load_ctb_flags()
simFlags.SimLayout='ctbh8_combined'
simFlags.LArEMBenergyCor=True 
simFlags.Eta = 0.2

# Re-writes the constant magnetic field for the MBPSID with the map.  
simFlags.MagFieldMap='mbps1-all-id-800-mbps2-muons-800.data'
simFlags.MagFieldMapCalculation=1

#--- Generator flags ------------------------------------------

simFlags.CalibrationRun.set_Value('LAr+Tile')

#--- Use single particle gun ----------------------------
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 22
pg.sampler.mom = PG.EEtaMPhiSampler(energy=20000, eta=[-3,3])
topSeq += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 4

## Use verbose G4 tracking
#from G4AtlasApps.SimFlags import simFlags
#simFlags.G4Commands += ['/tracking/verbose 1']

include("G4AtlasApps/G4Atlas.flat.configuration.py")


#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================
## Populate alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

#--- End jobOptions.G4Ctb_Sim.py file  -----------------------
