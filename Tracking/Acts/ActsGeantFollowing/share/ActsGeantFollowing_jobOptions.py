#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantFollower in ID (and MS)
#		It can be run using athena.py
#
#==============================================================


#--- Algorithm sequence ---------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
#topSeq.ContinueEventloopOnFPE = True
from RecExConfig.RecFlags import rec as rec
rec.doFloatingPointException.set_Value_and_Lock(True)
rec.doNameAuditor = True

#--- Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL) ----
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Constants import VERBOSE, INFO
ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 20000000

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags


### pass arguments with athena -c "..." ...jobOptions.py:
if 'myMomentum' not in dir() :
#    myMomentum = 30000
   myMomentum = 50000
#    myMomentum = 100000
#    myMomentum = 500000

if 'myRandomOffset' not in dir() :
    myRandomOffset = 0

if 'myRandomSeed1' not in dir() :
    # myRandomSeed1 = 12398190
    myRandomSeed1 = 123456

if 'myRandomSeed2' not in dir() :
    myRandomSeed2 = 820189

if 'myMaxEvent' not in dir():
    myMaxEvent = 100
    # myMaxEvent = 1000
    # myMaxEvent = 5000

if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    myGeo = 'ATLAS-R2-2016-00-00-00'


# Set everything to ATLAS
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff() # Muons not available yet with ACTS

# the global flags

# select latest Muon lay out on top of GEO-20
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()


from IOVDbSvc.CondDB import conddb

if myGeo.split('-')[1] == 'GEO' :
    if myGeo.split('-')[2] > 16 :
        globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-05-02'
        globalflags.ConditionsTag = 'OFLCOND-MC12-SDR-06'
    else :
        globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-04-15'
    print(globalflags.ConditionsTag)
elif myGeo.split('-')[1] == 'IBL' :
    globalflags.ConditionsTag = 'OFLCOND-SDR-BS14T-IBL-03'
else :
    globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-05-02'

print(globalflags.ConditionsTag)

DetFlags.Truth_setOn()

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()   ### is this necessary?
athenaCommonFlags.PoolHitsOutput.set_Off()
athenaCommonFlags.EvtMax =  myMaxEvent

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags() # Going to use an ATLAS layout
simFlags.SimLayout = myGeo
simFlags.EventFilter.set_Off()
simFlags.RunNumber = 284500  # MC16a run number used for all Run2 simulation

include("GeneratorUtils/StdEvgenSetup.py")

myMinEta =  -2.4
myMaxEta =  2.4

# myPDG = 11    # Electron
myPDG = 13  # Muon
# myPDG = 998 # Charged Geantino
# myPDG = 999 # neutral Geantino

# sept 2014 run ParticleGun
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = myPDG
# pg.sampler.mom = PG.EEtaMPhiSampler(energy=myMomentum, eta=[myMinEta,myMaxEta])
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=myMomentum, eta=[myMinEta,myMaxEta])
topSeq += pg

simFlags.RandomSeedOffset = myRandomOffset

simFlags.RandomSeedList.addSeed( "SINGLE", myRandomSeed1, myRandomSeed2 )

from RngComps.RngCompsConf import AtRndmGenSvc
myAtRndmGenSvc = AtRndmGenSvc()
myAtRndmGenSvc.Seeds = ["SINGLE "+str(myRandomSeed1)+" "+str(myRandomSeed2) ]
myAtRndmGenSvc.OutputLevel = VERBOSE
myAtRndmGenSvc.EventReseeding   = False
ServiceMgr += myAtRndmGenSvc

# ToolSvc setup
from AthenaCommon.AppMgr import ToolSvc

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

# SET UP ALIGNMENT CONDITIONS ALGORITHM
from IOVSvc.IOVSvcConf import CondSvc
ServiceMgr += CondSvc( OutputLevel=INFO )
from ActsGeometry import ActsGeometryConf

condSeq += ActsGeometryConf.NominalAlignmentCondAlg("NominalAlignmentCondAlg",
                                                     OutputLevel=INFO)

# We need the Magnetic fiels
import MagFieldServices.SetupField

# set up and configure the acts geometry construction
from ActsGeometry.ActsGeometryConf import ActsTrackingGeometrySvc
trkGeomSvc = ActsTrackingGeometrySvc("ActsTrackingGeometrySvc")
# used for the proxies during material mapping
trkGeomSvc.OutputLevel = INFO
trkGeomSvc.BuildSubDetectors = [
  "Pixel",
  "SCT",
  "TRT",
  "Calo"
]
from AthenaConfiguration.ComponentFactory import CompFactory
trkGeomSvc.CaloVolumeBuilder = CompFactory.ActsCaloTrackingVolumeBuilder()
trkGeomSvc.BuildBeamPipe = True
trkGeomSvc.UseMaterialMap = True
trkGeomSvc.MaterialMapInputFile = "/eos/project-a/acts/public/MaterialMaps/ATLAS-material-maps.json"
ServiceMgr += trkGeomSvc

# sets up the extrapolation tool
# this sets up the tracking geometry svc through the tracking geometry tool
ActsExtrapolator = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool")
ActsExtrapolator.OutputLevel = INFO
ActsExtrapolator.FieldMode = "ATLAS"
ActsExtrapolator.MaxSteps = 1000000

ActsExtrapolator.InteractionMultiScatering = True
ActsExtrapolator.InteractionEloss = True
ActsExtrapolator.InteractionRecord = True
if myPDG == 998 or myPDG == 999 :
    ActsExtrapolator.InteractionMultiScatering = FALSE
    ActsExtrapolator.InteractionEloss = FALSE

# The extrapolation tool accesses the trackinggeometry service
# through this tool. This tool has the conditions dependencies
# on the alignment GeoAlignmentStores (pseudo-alignment only right now).
# For each event, the GAS for the IOV needs to be set from the algorithm.
trkGeomTool = CfgMgr.ActsTrackingGeometryTool("ActsTrackingGeometryTool")
trkGeomTool.OutputLevel = INFO
ToolSvc += trkGeomTool

ActsExtrapolator.TrackingGeometryTool = trkGeomTool
ToolSvc += ActsExtrapolator


from TrkExEngine.AtlasExtrapolationEngine import AtlasExtrapolationEngine
ExtrapolationEngine = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas')
ToolSvc += ExtrapolationEngine

from ActsGeantFollowing.ActsGeantFollowingConf import ActsGeantFollowerHelper
ActsGeantFollowerHelper = ActsGeantFollowerHelper(name="ActsGeantFollowerHelper")
ActsGeantFollowerHelper.ActsExtrapolator         = ActsExtrapolator
ActsGeantFollowerHelper.ExtrapolationEngine      = ExtrapolationEngine
ActsGeantFollowerHelper.ExtrapolateDirectly      = False
ActsGeantFollowerHelper.ExtrapolateIncrementally = False
ActsGeantFollowerHelper.OutputLevel = INFO
ToolSvc += ActsGeantFollowerHelper

simFlags.OptionalUserActionList.addAction('ActsGeantFollowerTool')

############### The output collection #######################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "val DATAFILE='GeantFollowing.root' TYPE='ROOT' OPT='RECREATE'" ]

##############################################################


include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Populate alg sequence
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

# Conditions sequence for Athena MT
from AthenaCommon.AlgSequence import AthSequencer
if not hasattr(condSeq, "BeamSpotCondAlg"):
   from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
   condSeq += BeamSpotCondAlg( "BeamSpotCondAlg" )

# TestSTEP_Propagator.Straggling = False

# if myPDG == 998 or myPDG == 999 :
#     TestSTEP_Propagator.MultipleScattering = False
#     TestSTEP_Propagator.EnergyLoss = False

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
#--- End jobOptions GeantFollowing_jobOptions.py file  ------------------------------
