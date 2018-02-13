#--------------------------------------------------------------
# Geometry test jobOptions: SLHC 
#
# There are two geometry tests available. Only one can be run at a time:
#   doFastCheck True:  Runs the "GeometryCheck".
#                      This is a faster checker and finds most overlaps.
#   doFastCheck False: Runs the "recursive_test".
#                      This takes a lot longer but sometimes finds overlaps
#                      the other misses.
#
# It is recommended to run GeometryCheck while developing the geometry
# as it will find most of the overlaps and to run the recursive_test
# before using the geometry in production as an extra check.
#
#-----------------------------------------------------------------------------------------------------------------------

doFastCheck = True


from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-GEO-06-00-00')

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.TRT_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.simulate.Truth_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput="singleMuon_test_slhc_Hits.pool.root"
athenaCommonFlags.EvtMax=1

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout='ATLAS-SLHC-01-00-00_VALIDATION'
simFlags.SimLayout.set_On() 
simFlags.EventFilter.set_On()


## Run ParticleGenerator
athenaCommonFlags.PoolEvgenInput.set_Off()
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

import AthenaCommon.AtlasUnixGeneratorJob

import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 13
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=50000, eta=[-3,3])
topSeq += pg

try:
    include("G4AtlasApps/fragment.SimCopyWeights.py")
except:
    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
    topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add G4 sim framework alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

# SLHC Flags -----------------------------------------------------
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
#SLHC_Flags.SLHC_Version = 'SLHC-19-20'
SLHC_Flags.SLHC_Version = ''

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

#--------------------------------------------------------------
# Geometry Check
#--------------------------------------------------------------

theApp.initialize()

if (doFastCheck) :
    AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetGeometryCheck(1000)
    AtlasG4Eng.G4Eng.init_Simulation(3)

else :
    AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
    ##AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_start 0")
    ##AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursion_depth 2")
    AtlasG4Eng.G4Eng._ctrl.G4Command("/geometry/test/recursive_test")


# forcing the exit after the geo test
theApp.exit()
