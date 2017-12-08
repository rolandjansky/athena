#==============================================================
#
# Job options file for Geant4 Simulations of R-hadrons
#
# Models are :
# "generic": Eur.Phys.J.C50:353-362,2007
# "regge": arXiv:0909.5104arXiv:0909.5104
#
# Prerequisites for your run-dir:
# - PhysicsConfiguration.txt:
#   Three variations exist in this package:
#    - PhysicsConfiguration_generic.txt
#    - PhysicsConfiguration_regge.txt
#    - PhysicsConfiguration_regge_nomix.txt
# - particles.txt: Can be get_file'd from particles_<model>_<sparticle>_<mass>GeV.txt in this package
# - PDGTABLE.MeV: Get as PDGTABLE_<model>_<sparticle>_<mass>GeV.MeV
# - ProcessList.txt: Get as ProcessList_<model>_<sparticle>.txt
#
# For instructions on how to run with R-hadron decays see further down
#==============================================================

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.simulate.Truth_setOn()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'atlas_MyOutputFile.root'
athenaCommonFlags.EvtMax = 3

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS7T-04-03"

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.SimLayout.set_On()
SimFlags.EventFilter.set_On()

## Run ParticleGenerator
import AthenaCommon.AtlasUnixGeneratorJob
print "Particle PDG code is ", PDGcode
spgorders = ['pdgcode: constant 1009213',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: constant 0.5',
             'phi: flat  0 6.28318',
             'energy: constant 1300975'] # 300975  # 301000

## If you want decays, place decays.txt in your run-dir and uncomment this block
## to define the neutralino and set the neutralino mass to something sensible
#def enable_rhadron_decays():
#    from G4AtlasApps import AtlasG4Eng
#    AtlasG4Eng.G4Eng._ctrl.load("Gauginos")
#    lsp = AtlasG4Eng.G4Eng.gbl.ParticleDataModifier("s_chi_0_1")
#    lsp.SetParticleMass(150*GeV)
#    lsp.Stable(True)
#SimFlags.initFunctions.add_function("preInitPhysics", enable_rhadron_decays)

## Load RHadrons library in the sim init flow
def load_rhadrons_lib():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.load_Lib("RHadrons")
SimFlags.initFunctions.add_function("preInitPhysics", load_rhadrons_lib)

## Populate alg sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
