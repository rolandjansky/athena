#==============================================================
#
# Job options file for Geant4 Simulations
#
# Atlas simulation 
#
__version__="$Revision: 1.3 $"
#==============================================================

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.LAr_setOff()
DetFlags.Muon_setOff()
#
#DetFlags.simulate.Truth_setOn()
#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput='atlas_MyOutputFile.root'
athenaCommonFlags.EvtMax=0

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.import_JobProperties('G4AtlasApps.atlas_flags')
SimFlags.SimLayout='ATLAS-CSC-05-00-00' # specific value 
#SimFlags.SimLayout.set_On()              # uses the default value 

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.TileVersionOverride = 'TileCal-CSC-03'

SimFlags.PhysicsList.set_Value("Fast_Physics")

# - uses single particle generator
SimFlags.KinematicsMode='SingleParticle'
SimFlags.ParticlePDG='11'
#   set energy constant to 10 GeV
SimFlags.Energy=10000

#  sets the EtaPhi, VertexSpread and VertexRange checks on
SimFlags.EventFilter.set_On()

SimFlags.init_Level=1

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 9


#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================
from G4AtlasApps import SimKernel

# enter interactive mode 
theApp.initialize()

print "START0><"
AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetGeometryCheck(1000)	
AtlasG4Eng.G4Eng.init_Simulation(3)
print "END><"

# AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
# AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_start 0")
# AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_depth 2")
# AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursive_test")

# forcing the exit after the geo test 
theApp.exit() 

#--- End test_ATLAS_GeoTest.py file  ------------------------------
