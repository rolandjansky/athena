#==============================================================
#
# Job options file for Geant4 Simulations of Monopoles
#
#
# Prerequisites for your run-dir:
# - particles.txt: Can be get_file'd from particles_<model>_<sparticle>_<mass>GeV.txt in this package
# - PDGTABLE.MeV: Get as PDGTABLE_<model>_<sparticle>_<mass>GeV.MeV
#
#==============================================================

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = outputfile
print "Output will be saved to ", outputfile
athenaCommonFlags.EvtMax = evtmax
print "Number of requested events is ", evtmax

# Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS7T-04-03"

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
#SimFlags.MagneticField.set_Off()
SimFlags.EventFilter.set_On()
SimFlags.SimLayout.set_On()

## Use single particle generator
import AthenaCommon.AtlasUnixGeneratorJob
print "Particle PDG code is ", PDGcode
spgorders = ['pdgcode: constant '+str(PDGcode),
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: constant 0.5',
             'phi: flat  0 6.28318',
             'pt: constant 1000000']

## Use the monopole equation of motion for B field propagation
SimFlags.EquationOfMotion.set_On()
SimFlags.EquationOfMotion = "MonopoleEquationOfMotion"

## Load Monopole library in the sim init flow
def load_monopole_lib():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.load_Lib("Monopole")
SimFlags.initFunctions.add_function("preInitPhysics", load_monopole_lib)
#SimFlags.G4Commands += ['/tracking/verbose 1']

SimFlags.G4Stepper = 'ClassicalRK4'

def setup_hipkiller():
    from G4AtlasApps import PyG4Atlas,AtlasG4Eng
    myHipKiller = PyG4Atlas.UserAction('G4UserActions', 'HIPKiller', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myHipKiller)
SimFlags.InitFunctions.add_function("postInit", setup_hipkiller)

#def setup_looperkiller():
#    from G4AtlasApps import PyG4Atlas,AtlasG4Eng
#    myLooperKiller = PyG4Atlas.UserAction('G4UserActions', 'LooperKiller', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
#    myLooperKiller.set_Properties({"MaxSteps":"2000000","PrintSteps":"2","VerboseLevel":"0"})
#    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myLooperKiller)

#SimFlags.InitFunctions.add_function("postInit", setup_looperkiller)

## Populate alg sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
