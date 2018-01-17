#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantinoMapping.
#		It can be run using athena.py
#
__version__="$Revision: 706440 $"
#==============================================================

#--- Algorithm sequence ---------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#- Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL) ------
#from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

#--- Include JobSpecs.py --------------------------------------
#include ('JobSpecs.py')

### pass arguments with athena -c "..." ...jobOptions.py:
if 'myMomentum' not in dir() :
    myMomentum = 10000

if 'myRandomSeed' not in dir() :
    myRandomSeed = 12398190

if 'myMaxEvent' not in dir() :
    myMaxEvent = 10
    
if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    myGeo = ''

JobSpecs_DetLayout = myGeo
JobSpecs_Events    = myMaxEvent
JobSpecs_Seed1     = myRandomSeed


#include("InDetSLHC_Example/preInclude.OverrideBFieldTag.py")

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = JobSpecs_DetLayout

include("InDetSLHC_Example/preInclude.SiliconOnly.py")
    
globalflags.ConditionsTag   = 'OFLCOND-ATLAS-HL-LHC-00'

DetFlags.simulate.Truth_setOn()

# specify your "/tmp/<username>/" directory here (output files may be large!)
outPath = "" 

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = outPath + 'Hits.pool.root'
athenaCommonFlags.EvtMax = JobSpecs_Events

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On() 
# simFlags.SimLayout = 'ATLAS-SLHC-01-01-00_VALIDATION'
simFlags.SimLayout='ATLAS-SLHC-01-03-00_VALIDATION'
simFlags.RandomSeed=JobSpecs_Seed1
#  sets the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_Off()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 999 #Geantino
if 'pt' == str(myPt):
    pg.sampler.mom = PG.PtEtaMPhiSampler(pt=myMomentum, eta=[-5,5])
else:
    print 'not sure what you want to do here'
#pg.sampler.mom = PG.EEtaMPhiSampler(energy=50000, eta=[-3,3])
topSeq += pg
print topSeq.ParticleGun

try:
    include("G4AtlasApps/fragment.SimCopyWeights.py")
except:
    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
    topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")

## Add an action
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::MaterialStepRecorderTool'['Run','Event','Step'])

# suppress the enormous amount of MC output
from TruthExamples.TruthExamplesConf import DumpMC
DumpMC.VerboseOutput = False

############### The Material hit collection ##################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# --- check dictionary
ServiceMgr.AthenaSealSvc.CheckDictionary   = True
# --- commit interval (test)
ServiceMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10
MaterialStream              = AthenaPoolOutputStream ( 'MaterialStream' )
MaterialStream.OutputFile   = outPath + "MaterStepFile.root" 
MaterialStream.ItemList    += [ 'MaterialStepVector#*']

##############################################################

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Populate alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

from InDetIBL_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

#--- End jobOptions.GeantinoMapping.py file  ------------------------------

