#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantinoMapping.
#		It can be run using athena.py
#
__version__="$Revision: 467755 $"
#==============================================================


#--- Algorithm sequence ---------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL) ----
#from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 20000

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

from G4AtlasApps import AtlasG4Eng
AtlasG4Eng.G4Eng._ctrl.fldMenu.UseStepper('NystromRK4')

#--- Include JobSpecs.py --------------------------------------
#include ('JobSpecs.py')

### pass arguments with athena -c "..." ...jobOptions.py:
if 'myMomentum' not in dir() :
    myMomentum = 5000

if 'myRandomOffset' not in dir() :
    myRandomOffset = 0

if 'myRandomSeed1' not in dir() :
    myRandomSeed1 = 12398190

if 'myRandomSeed2' not in dir() :
    myRandomSeed2 = 820189

if 'myMaxEvent' not in dir() :
    myMaxEvent = 25

if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    myGeo = 'ATLAS-GEO-18-01-00'


# Set everything to ATLAS
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
# the global flags
if myGeo.split('-')[1] == 'GEO' :
    if myGeo.split('-')[2] > 16 :
        globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-05-02'
    else :
        globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-04-15'
    print globalflags.ConditionsTag
elif myGeo.split('-')[1] == 'IBL' :
    globalflags.ConditionsTag = 'OFLCOND-SDR-BS14T-IBL-03'
else :
    globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-05-02'

print globalflags.ConditionsTag

#from AthenaCommon import AthenaCommonFlags
#from BFieldAth.BFieldAthConf import MagFieldAthenaSvc
#ServiceMgr += MagFieldAthenaSvc(SetupCOOL=True, NameOfTheSource='COOL',
#                                UseDCS=not AthenaCommonFlags.isOnline())

#from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09')

#DetFlags.simulate.Truth_setOn()   ### deprecated!
DetFlags.Truth_setOn()

# specify your "/tmp/<username>/" directory here (output files may be large!)
outPath = ""  #"/tmp/wlukas/"

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()   ### is this necessary?
athenaCommonFlags.PoolHitsOutput = outPath + 'Hits.pool.root'
athenaCommonFlags.EvtMax = myMaxEvent

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags() # Going to use an ATLAS layout
SimFlags.SimLayout = myGeo
SimFlags.EventFilter.set_Off()

myMinEta =  0.0
myMaxEta =  0.0

myPDG    = 998  # 999 = Geantinos, 13 = Muons, 0 diferently charged geantino

## Run ParticleGenerator
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = [ 'pdgcode:' + ' constant ' + str(myPDG),
              'vertX:'   + ' constant 0.0',
              'vertY:'   + ' constant 0.0',
              'vertZ:'   + ' constant 0.0',
              't:'       + ' constant 0.0',
              'eta:'     + ' flat ' + str(myMinEta) + ' ' + str(myMaxEta),
              'phi:'     + ' flat  0 6.2831853',
              myPt + ':' + ' constant ' + str(myMomentum) ]
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)

SimFlags.RandomSeedOffset = myRandomOffset

### new rel17 (check Simulation/G4Atlas/G4AtlasApps/python/SimFlags.py for details)
# SimFlags.RandomSeedList.addSeed( "SINGLE", myRandomSeed1, myRandomSeed2 )

### alternative for earlier rel17/rel16
from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
ServiceMgr += AtRanluxGenSvc()
ServiceMgr.AtRanluxGenSvc.Seeds = [ "SINGLE " + str(myRandomSeed1) + ' ' + str(myRandomSeed2) ]
### alternative to previous line
#ServiceMgr.AtRanluxGenSvc.Seeds = [ "SINGLE OFFSET " + str(myRandomOffset) + ' ' + str(myRandomSeed1) + ' ' + str(myRandomSeed2) ]

### alternative for old rel16 versions and before (?)
#SeedString = "SINGLE " + str(myRandomSeed1) + ' ' + str(myRandomSeed2)
#SimFlags.Seeds.set_Value(SeedString)

#AtlasG4Eng.G4Eng._ctrl.fldMenu.UseStepper('AtlasRK4')

# suppress the enormous amount of MC output
from TruthExamples.TruthExamplesConf import DumpMC
DumpMC.VerboseOutput = False

# ToolSvc setup
from AthenaCommon.AppMgr import ToolSvc

# Estrapolator setup
include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
AtlasTrackingGeometrySvc = svcMgr.AtlasTrackingGeometrySvc

#--------------------------------------------------------------
# Assign the TrackingGeometry to the Algorithm
#--------------------------------------------------------------

# the layer material inspector
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerMaterialInspector
LayerMaterialInspector = Trk__LayerMaterialInspector(name= 'LayerMaterialInspector')
LayerMaterialInspector.OutputLevel = INFO
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += LayerMaterialInspector

# the tracking volume displayer
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeDisplayer
TrackingVolumeDisplayer = Trk__TrackingVolumeDisplayer(name= 'TrackingVolumeDisplayer')
TrackingVolumeDisplayer.TrackingVolumeOutputFile = 'TrackingVolumes-'+TrkDetFlags.MaterialMagicTag()+'.C'
TrackingVolumeDisplayer.LayerOutputFile          = 'Layers-'+TrkDetFlags.MaterialMagicTag()+'.C'
TrackingVolumeDisplayer.SurfaceOutputFile        = 'Surfaces-'+TrkDetFlags.MaterialMagicTag()+'.C'
ToolSvc += TrackingVolumeDisplayer

# set up the Geometry Builder test
from TrkDetDescrAlgs.TrkDetDescrAlgsConf import Trk__GeometryBuilderTest
GeometryBuilderTest = Trk__GeometryBuilderTest(name ='GeometryBuilderTest')
GeometryBuilderTest.TrackingGeometrySvc     = AtlasTrackingGeometrySvc
GeometryBuilderTest.DisplayTrackingVolumes  = False
GeometryBuilderTest.RecordLayerMaterial     = False
GeometryBuilderTest.LayerMaterialInspector  = LayerMaterialInspector
GeometryBuilderTest.TrackingVolumeDisplayer = TrackingVolumeDisplayer
GeometryBuilderTest.WriteNtuple             = False
GeometryBuilderTest.OutputLevel             = INFO
topSeq += GeometryBuilderTest

# PROPAGATOR DEFAULTS --------------------------------------------------------------------------------------

TestPorpagators  = []

from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
#from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as Propagator
TestPropagator = Propagator(name = 'TestPropagator')
ToolSvc += TestPropagator

TestPorpagators += [ TestPropagator ]

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as STEP_Propagator
TestSTEP_Propagator = STEP_Propagator(name = 'TestSTEP_Propagator')
ToolSvc += TestSTEP_Propagator

TestPorpagators += [TestSTEP_Propagator]

# UPDATOR DEFAULTS -----------------------------------------------------------------------------------------

TestUpdators    = []

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
TestMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'TestMaterialEffectsUpdator')
TestMaterialEffectsUpdator.EnergyLoss           = False
TestMaterialEffectsUpdator.MultipleScattering   = False
ToolSvc += TestMaterialEffectsUpdator

TestUpdators    += [ TestMaterialEffectsUpdator ]

TestMaterialEffectsUpdatorLandau = MaterialEffectsUpdator(name = 'TestMaterialEffectsUpdatorLandau')
TestMaterialEffectsUpdatorLandau.LandauMode           = True
TestMaterialEffectsUpdatorLandau.EnergyLoss           = False
TestMaterialEffectsUpdatorLandau.MultipleScattering   = False
ToolSvc += TestMaterialEffectsUpdatorLandau

TestUpdators    += [ TestMaterialEffectsUpdatorLandau ]

# the UNIQUE NAVIGATOR ( === UNIQUE GEOMETRY) --------------------------------------------------------------
from TrkExTools.TrkExToolsConf import Trk__Navigator
TestNavigator = Trk__Navigator(name = 'TestNavigator')
TestNavigator.TrackingGeometrySvc = AtlasTrackingGeometrySvc
ToolSvc += TestNavigator

# CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

TestSubPropagators = []
TestSubUpdators = []

# -------------------- set it depending on the geometry ----------------------------------------------------
# default for ID is (Rk,Mat)
TestSubPropagators += [ TestPropagator.name() ]
TestSubUpdators    += [ TestMaterialEffectsUpdator.name() ]

# default for Calo is (Rk,MatLandau)
TestSubPropagators += [ TestPropagator.name() ]
TestSubUpdators    += [ TestMaterialEffectsUpdatorLandau.name() ]

# default for MS is (STEP,Mat)
TestSubPropagators += [ TestSTEP_Propagator.name() ]
TestSubUpdators    += [ TestMaterialEffectsUpdator.name() ]
# ----------------------------------------------------------------------------------------------------------

# call the base class constructor
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
TestExtrapolator = Trk__Extrapolator('TextExtrapolator',\
                           Navigator = TestNavigator,\
                           MaterialEffectsUpdators = TestUpdators,\
                           Propagators = TestPorpagators,\
                           SubPropagators = TestSubPropagators,\
                           SubMEUpdators = TestSubUpdators,\
                           DoCaloDynamic = False)
ToolSvc += TestExtrapolator

############### The output collection #######################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# --- check dictionary
ServiceMgr.AthenaSealSvc.CheckDictionary   = True
# --- commit interval (test)
ServiceMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10

from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "val DATAFILE='GeantFollowing.root' TYPE='ROOT' OPT='RECREATE'" ]

##############################################################

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

from AthenaCommon.CfgGetter import getPublicTool
ServiceMgr.UserActionSvc.BeginOfEventActions += [getPublicTool("GeantFollower")]
ServiceMgr.UserActionSvc.EndOfEventActions += [getPublicTool("GeantFollower")]
ServiceMgr.UserActionSvc.SteppingActions += [getPublicTool("GeantFollower")]

#--- End jobOptions.GeantinoMapping.py file  ------------------------------
