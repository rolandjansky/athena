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
ServiceMgr.MessageSvc.defaultLimit = 10000

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

#--- Include JobSpecs.py --------------------------------------
#include ('JobSpecs.py')

### pass arguments with athena -c "..." ...jobOptions.py:
if 'myMomentum' not in dir() :
    myMomentum = 10000

if 'myRandomOffset' not in dir() :
    myRandomOffset = 0

if 'myRandomSeed1' not in dir() :
    myRandomSeed1 = 12398190

if 'myRandomSeed2' not in dir() :
    myRandomSeed2 = 820189

if 'myMaxEvent' not in dir() :
    myMaxEvent = 10

if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    myGeo = 'ATLAS-GEO-18-01-03'


# Set everything to ATLAS
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
# the global flags
if myGeo.split('-')[1] == 'GEO' :
    if myGeo.split('-')[2] > 16 :
        globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-05-02'
    else :
        globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-04-15'
elif myGeo.split('-')[1] == 'IBL' :
    globalflags.ConditionsTag = 'OFLCOND-SDR-BS14T-IBL-03'
else :
    globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-05-02'

print globalflags.ConditionsTag

#DetFlags.simulate.Truth_setOn()   ### deprecated!
DetFlags.Truth_setOn()

# specify your "/tmp/<username>/" directory here (output files may be large!)
outPath = ""  #"/tmp/wlukas/"

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()   ### is this necessary?
athenaCommonFlags.PoolHitsOutput = '/dev/null' # outPath + 'Hits.pool.root'
athenaCommonFlags.EvtMax = myMaxEvent

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags() # Going to use an ATLAS layout
SimFlags.SimLayout = myGeo  # specific value
SimFlags.EventFilter.set_Off()

myMinEta = -2.7
myMaxEta =  2.7

myPDG    = 13   # 999 = Geantinos, 13 = Muons

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
SimFlags.RandomSeedList.addSeed( "SINGLE", myRandomSeed1, myRandomSeed2 )

### alternative for earlier rel17/rel16
#from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
#ServiceMgr += AtRanluxGenSvc()
#ServiceMgr.AtRanluxGenSvc.Seeds = [ "SINGLE " + str(myRandomSeed1) + ' ' + str(myRandomSeed2) ]
### alternative to previous line
#ServiceMgr.AtRanluxGenSvc.Seeds = [ "SINGLE OFFSET " + str(myRandomOffset) + ' ' + str(myRandomSeed1) + ' ' + str(myRandomSeed2) ]

### alternative for old rel16 versions and before (?)
#SeedString = "SINGLE " + str(myRandomSeed1) + ' ' + str(myRandomSeed2)
#SimFlags.Seeds.set_Value(SeedString)


## Add an action
def eloss_action():
    from G4AtlasApps import AtlasG4Eng,PyG4Atlas
    EnergyLossAction = PyG4Atlas.UserAction('TrkG4UserActions','EnergyLossRecorder', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    EnergyLossAction.set_Properties({"verboseLevel":"0",
#                                 "cylinders" : "5",
#                                 "R1": "36.5", "Z1": "2748",   # for Beate ... 34.5 is too small b/c BeamPipe is shifted/tilted!
#                                 "R2":  "250", "Z2": "3000",   # for Beate ... was z=680 but PP0 PST extends much further
#                                 "R3":  "550", "Z3": "2820",   # for Beate
#                                 "R4": "1085", "Z4": "3120",   # for Beate ... or z=3242
#                                 "R5": "1170", "Z5": "3520",   # for Beate ... Pixel support until z=3460 (or 3470) / ID end-plate until z=3512?
#                                 "R6":"13000", "Z6":"22500",   # ATLAS
#                                 "cylinders" : "3",
#                                 "R1":  "600", "Z1": "2700",   # for Beate (old)
#                                 "R2": "1170", "Z2": "2748",   # for Beate (old)
#                                 "R3": "4250", "Z3": "6527",   # for Beate (old) (ATLAS: z=6500)
#                                 "cylinders" : "6",
#                                 "R1": "4250", "Z1": "6527",   # for Niels
#                                 "R2": "4440", "Z2": "7100",   # for Niels
#                                 "R3": "6200", "Z3": "8000",   # for Niels
#                                 "R4": "8500", "Z4":"10500",   # for Niels
#                                 "R5":"11500", "Z5":"16000",   # for Niels
#                                 "R6":"12000", "Z6":"22030",   # for Niels
                                 "cylinders" : "6",
                                 "R1": "36.5", "Z1": "2748",   # for Niels and Peter (rel16/rel17 validation)
                                 "R2":  "250", "Z2": "2750",   # for Niels and Peter (rel16/rel17 validation)
                                 "R3":  "550", "Z3": "2820",   # for Niels and Peter (rel16/rel17 validation)
                                 "R4": "1085", "Z4": "3120",   # for Niels and Peter (rel16/rel17 validation)
                                 "R5": "1170", "Z5": "3520",   # for Niels and Peter (rel16/rel17 validation)
                                 "R6": "4250", "Z6": "6527",   # for Niels and Peter (rel16/rel17 validation)
                                 "minEta"   : str(myMinEta),
                                 "maxEta"   : str(myMaxEta),
                                 "momentum" : str(myMomentum),
                                 "pdg"      : str(myPDG),
                                 "usePt"    : myPt,
                                 "useX0"    : "on",
                                 "onionMode": "on",
                                 "keepAlive": "off"
                                  })
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(EnergyLossAction)

SimFlags.InitFunctions.add_function('preInitG4', eloss_action)

# suppress the enormous amount of MC output
from TruthExamples.TruthExamplesConf import DumpMC
DumpMC.VerboseOutput = False


##############################################################

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

#--- End EnergyLossSimulation_jobOptions.py file  ------------------------------

