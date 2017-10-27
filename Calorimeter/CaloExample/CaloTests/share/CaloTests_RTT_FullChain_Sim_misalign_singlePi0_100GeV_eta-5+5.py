#==============================================================
#
# Job options file for Geant4 Simulations
#
# Atlas simulation 
#
#==============================================================

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

#---- Global conditions tag -----------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = 'OFLCOND-SIM-BS7T-00'

#--- AthenaCommon flags -----------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()

#--- Simulation -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
#bugs 73445 Comment #10
SimFlags.load_atlas_flags()
#SimFlags.SimLayout='ATLAS-GEO-06-01-00'

SimFlags.EventFilter.set_On()

# - uses single particle generator
import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 111',
             'vertX: constant 0.0',
             'vertY: constant 0.0',
             'vertZ: constant 0.0',
             't: constant 0.0',
             'eta: flat -5.0 5.0',
             'phi: flat  0 6.28318',
             'e: constant 100000']
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)
from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
ServiceMgr += AtRanluxGenSvc()
ServiceMgr.AtRanluxGenSvc.Seeds = ["SINGLE 2040160768 443921183"]

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3


#######################################################################

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

