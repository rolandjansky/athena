#
# This is based on Tracking/TrkG4Components/TrkG4UserActions/share/GeantinoMapping_jobOptions.py
# Copied 11/04/2018
#


#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantinoMapping.
#		It can be run using athena.py
#

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

import random
import time

### pass arguments with athena -c "..." ...jobOptions.py:
if 'myMomentum' not in dir() :
    myMomentum = 10000

if 'myRandomOffset' not in dir() :
    myRandomOffset = int(random.uniform(0,time.time())*0.001 )

if 'myRandomSeed1' not in dir() :
    myRandomSeed1 = int(random.uniform(0,time.time()))

if 'myRandomSeed2' not in dir() :
    myRandomSeed2 = int(random.uniform(0,time.time()))

if 'myMaxEvent' not in dir() :
    myMaxEvent = 100

if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    myGeo = 'ATLAS-R2-2016-00-00-00'

print 'Random seeds and offset as calcluated by jobOptions ', myRandomSeed1, ' ', myRandomSeed2, ' offset - ', myRandomOffset


# Set everything to ATLAS
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# the global flags
globalflags.ConditionsTag = 'OFLCOND-SIM-00-00-00'
print globalflags.ConditionsTag


#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()   ### is this necessary?
athenaCommonFlags.PoolHitsOutput = 'Hits.pool.root'
athenaCommonFlags.EvtMax = 1000000

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags() # Going to use an ATLAS layout
simFlags.SimLayout = myGeo
simFlags.EventFilter.set_Off()

myMinEta = -2.5
myMaxEta =  2.5

myPDG    = 999   # 999 = Geantinos, 13 = Muons

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 20000

import ParticleGun as PG
pg = PG.ParticleGun()
pg.sampler.pid = 999
pg.randomSeed = 123456
pg.sampler.mom = PG.EEtaMPhiSampler(energy=10000, eta=[myMinEta,myMaxEta])
topSeq += pg

simFlags.RandomSeedOffset = myRandomOffset
simFlags.RandomSeedList.addSeed( "SINGLE", myRandomSeed1, myRandomSeed2 )

from RngComps.RngCompsConf import AtRndmGenSvc
myAtRndmGenSvc = AtRndmGenSvc()
myAtRndmGenSvc.Seeds = ["SINGLE "+str(myRandomSeed1)+" "+str(myRandomSeed2) ]
myAtRndmGenSvc.OutputLevel 	= VERBOSE
myAtRndmGenSvc.EventReseeding   = False
ServiceMgr += myAtRndmGenSvc

# from TrkG4UserActions.TrkG4UserActionsConfig import getMaterialStepRecorderTool
# from AthenaCommon.CfgGetter import addTool
# addTool("TrkG4UserActions.TrkG4UserActionsConfig.getMaterialStepRecorderTool", "G4UA::MaterialStepRecorderTool")


# matStepRecTool = CfgMgr.G4UA__MaterialStepRecorderTool('G4UA::MaterialStepRecorderTool')
# matStepRecTool = getMaterialStepRecorderTool()
# toolSvc = CfgMgr.ToolSvc()
# toolSvc += matStepRecTool

# from G4AtlasServices.G4AtlasUserActionConfig import getUserActionSvc
# ServiceMgr += getUserActionSvc()

# add the material step recording action
simFlags.OptionalUserActionList.addAction('G4UA::MaterialStepRecorderTool')



# simFlags.UseV2UserActions = True
# simFlags.OptionalUserActionList.addAction('G4UA::MaterialStepRecorderTool',['BeginOfRun','BeginOfEvent','EndOfEvent','Step'])

############### The Material hit collection ##################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# --- commit interval (test)
ServiceMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
MaterialStream              = AthenaPoolOutputStream ( 'MaterialStream' )
MaterialStream.OutputFile   =   "MaterialStepFile.root"
MaterialStream.ItemList    += [ 'Trk::MaterialStepCollection#*']

##############################################################

include("G4AtlasApps/G4Atlas.flat.configuration.py")

# Add the beam effects algorithm
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Populate alg sequence
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

# Conditions sequence for Athena MT
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "BeamSpotCondAlg"):
   from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
   condSeq += BeamSpotCondAlg( "BeamSpotCondAlg" )


#--- End jobOptions.GeantinoMapping.py file  ------------------------------
