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

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
#simFlags.MagneticField.set_Off()
simFlags.EventFilter.set_On()
simFlags.SimLayout.set_On()

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
simFlags.EquationOfMotion.set_On()
simFlags.EquationOfMotion = "MonopoleEquationOfMotion"

## ## Load Monopole library in the sim init flow
## def load_monopole_lib():
##     from G4AtlasApps import AtlasG4Eng
##     AtlasG4Eng.G4Eng.load_Lib("Monopole")
## simFlags.initFunctions.add_function("preInitPhysics", load_monopole_lib)
## #simFlags.G4Commands += ['/tracking/verbose 1']

simFlags.G4Stepper = 'ClassicalRK4'
simFlags.OptionalUserActionList.addAction('G4UA::HIPKillerTool', ['Step'])

#simFlags.OptionalUserActionList.addAction('G4UA::LooperKillerTool', ['Step'])

## Populate alg sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
