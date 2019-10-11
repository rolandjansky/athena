#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantinoMapping.
#		It can be run using athena.py
#
# Modified version of Tracking/TrkG4Components/TrkG4UserActions/share/GeantinoMapping_jobOptions.py
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
    print "=== 'myMaxEvent' not specificed, defaulting to 100 events === " 
    myMaxEvent = 100

if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    print "=== 'myGeo' not specificed, defaulting to ATLAS-P2-ITK-20-03-00 === " 
    myGeo = 'ATLAS-P2-ITK-20-03-00'

# We need both the GeoTag and the sim layout. 
# Allow the user to be able to provide either with or w/o "_VALIDATION" transparently. 
myGeo = myGeo.rstrip("_VALIDATION")
mySimLayout = myGeo + "_VALIDATION"

print 'Random seeds and offset as calcluated by jobOptions ', myRandomSeed1, ' ', myRandomSeed2, ' offset - ', myRandomOffset

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = myGeo

# Set everything to ATLAS
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()



## Need to set the layout option up front
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

if globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-20'):
   SLHC_Flags.LayoutOption="InclinedDuals"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-19'):
   SLHC_Flags.LayoutOption="InclinedQuads"

elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-17'):
   SLHC_Flags.LayoutOption="InclinedAlternative"
   
elif globalflags.DetDescrVersion().startswith('ATLAS-P2-ITK-22'):
   SLHC_Flags.LayoutOption="InclinedAlternative"
   
print "SLHC_Flags.LayoutOption = ",SLHC_Flags.LayoutOption

include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")
# the global flags
globalflags.ConditionsTag = 'OFLCOND-SIM-00-00-00'
print globalflags.ConditionsTag


#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()   ### is this necessary?
athenaCommonFlags.PoolHitsOutput = 'Hits.pool.root'
athenaCommonFlags.EvtMax = myMaxEvent

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags() # Going to use an ATLAS layout
SimFlags.SimLayout = mySimLayout
SimFlags.EventFilter.set_Off()

myMinEta = -6.0
myMaxEta =  6.0

myPDG    = 999   # 999 = Geantinos, 13 = Muons

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = myMaxEvent

import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=SimFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = myPDG
pg.sampler.mom = PG.EEtaMPhiSampler(energy=10000, eta=[myMinEta,myMaxEta])
topSeq += pg

SimFlags.RandomSeedOffset = myRandomOffset

### new rel17 (check Simulation/G4Atlas/G4AtlasApps/python/SimFlags.py for details)
SimFlags.RandomSeedList.addSeed( "SINGLE", myRandomSeed1, myRandomSeed2 )

from RngComps.RngCompsConf import AtRndmGenSvc 
myAtRndmGenSvc = AtRndmGenSvc()
myAtRndmGenSvc.Seeds = ["SINGLE "+str(myRandomSeed1)+" "+str(myRandomSeed2) ] 
myAtRndmGenSvc.OutputLevel 	= VERBOSE
myAtRndmGenSvc.EventReseeding   = False
ServiceMgr += myAtRndmGenSvc

## add the material step recording action
SimFlags.OptionalUserActionList.addAction('G4UA::MaterialStepRecorderTool',['Run','Event','Step'])

############### The Material hit collection ##################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# --- check dictionary
ServiceMgr.AthenaSealSvc.CheckDictionary   = True
ServiceMgr.AthenaPoolCnvSvc.OutputLevel = INFO
# --- commit interval (test)
ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10
#output stream to write our material 
MaterialStream              = AthenaPoolOutputStream ( 'MaterialStream' )
MaterialStream.OutputFile   =   "MaterialStepCollection.root"
MaterialStream.ItemList    += [ 'Trk::MaterialStepCollection#*']

# Add the beam effects algorithm
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)
topSeq.BeamEffectsAlg.GenEventManipulators=[]

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=False)

include("InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py")
