#==============================================================
#
# Job options file for Geant4 Simulations
#
# Atlas simulation 
#
__version__="$Revision: 1.1 $"
#==============================================================

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.LAr_setOff()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
#
DetFlags.simulate.Truth_setOn()
#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolEvgenInput=['rfio:/castor/cern.ch/atlas/project/dc2/.../dc2.002930.mu_pt30_eta320.evgen805._0001.pool.root']
#athenaCommonFlags.PoolHitsOutput='atlas_MyOutputFile.pool.root'
athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.EvtMax=1000

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
# Look into SimFlags.SimLayout for other possible values 
SimFlags.SimLayout='ATLAS-CSC-02-00-00' # specific value 
#SimFlags.SimLayout.set_On()             # uses the default value 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.TileVersionOverride = 'TileCal-CSC-03'
#log.info( "GeoModelSvc.TileVersionOverride = %s" % (GeoModelSvc.TileVersionOverride) )

#  sets the EtaPhi, VertexSpread and VertexRange checks on
SimFlags.EventFilter.set_On()
#  sets LArParametrization 
#SimFlags.LArParameterization=2
#  No magnetic field 
SimFlags.MagneticField.set_Off()

# - uses single particle generator
SimFlags.KinematicsMode='SingleParticle'
SimFlags.ParticlePDG='999'
#   set energy constant to 10 GeV
#SimFlags.Energy=100 

# set your own particle generator orders here.
# do this for example if you need generation at fixed pt
# PDG code will be set following user instructions in the SimFlags.ParticlePDG
# SimFlags.Energy will be ingored if you uncomment the following lines

SimFlags.ParticleGeneratorOrders={'vertX:' : ' constant 1.0','vertY:' :' constant 1.0','vertZ:' : ' constant 0.0',
          't:' :' constant 0.0', 'eta:' : ' flat -1.7 1.7', 'phi:' : ' flat -3.14159 3.14159',
          'pdgcode:' : ' constant 999'}
#
#   'phi:' : ' flat  0. 6.28318'  ' flat  -3.14159 3.14159',  'phi:' : ' flat  3.3379 4.1233' - special
#   'pt:' : ' constant 50000'
#   'pdgcode:' : ' sequence -211 211'
#
# - reads events already generated

#SimFlags.KinematicsMode='ReadGeneratedEvents'
# (the input file name is athenaCommonFlags.PoolEvgenInput]
# - uses a given generator 
#SimFlags.KinematicsMode='EventGenerator' 
#SimFlags.GeneratorPath='G4AtlasApps/Z-mumuOptions.py')

#from G4AtlasApps import AtlasG4Eng

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

SimFlags.PhysicsList.set_Value('Fast_Physics')

#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["intLen DATAFILE='intLength.root' OPT='NEW'"];

from G4AtlasApps import SimKernel

# enter interactive mode 
theApp.initialize()

# include("G4UserActions/RadLengthIntegrator_options.py")
include("G4UserActions/IntLengthIntegrator_options.py")

#G4Command=AtlasG4Eng.G4Eng.gbl.G4Commands()
#G4Command.tracking.verbose(1)

# start run after the interactive mode 
#AtlasG4Eng.G4Eng.init_Simulation(3)
theApp.nextEvent( theApp.EvtMax )
theApp.finalize()
theApp.exit()

#--- End jobOptions.G4Atlas_Sim.py file  ------------------------------
