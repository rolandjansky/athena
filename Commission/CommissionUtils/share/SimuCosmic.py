#==============================================================
#
# Job options file for Geant4 Simulations
#
# Atlas+Cavern cosmic simulation 
#
#==============================================================

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
#
DetFlags.simulate.Truth_setOn()

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.import_Flags('atlas_flags')

SimFlags.MagneticField.set_Value(False)

# SimFlags.WriteTTR.StatusOn  = True


SimFlags.SimLayout.set_Value('Commissioning-1')
SimFlags.PersistencyHit.set_Value('atlas_MyOutputFile.root')

# - uses a given generator 
SimFlags.KinematicsMode.set_Value('EventGenerator')
SimFlags.GeneratorPath.set_Value('G4AtlasApps/CosmicGenerator.py')

#--- Event related parameters ---------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000;

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 4

#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================
include("G4AtlasApps/G4Atlas_Sim.py")

# CosmicGenerator.yvert_val =   115200.5
# CosmicGenerator.yvert_val =   95000.
CosmicGenerator.emin = 5000;      # 5 GeV
CosmicGenerator.emax = 5000*1000; # 5 TeV
CosmicGenerator.PrintMod = 100000 

# MessageSvc.OutputLevel = DEBUG
CosmicGenerator.Radius=10000.
AtRndmGenSvc.Seeds = ["COSMICS 2040160768 443921183"]

# Run Number
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=9999
EventSelector.EventsPerRun=20000000 

Stream1.OutputFile = "cosmic.simul.pool.root"
# Stream2.OutputFile = "cosmic.ttr.pool.root" 

#--- End jobOptions.G4Cosmic.py file  -------------------------
