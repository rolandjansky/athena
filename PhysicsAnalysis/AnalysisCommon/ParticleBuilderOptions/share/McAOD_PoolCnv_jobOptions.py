# Load some Dlls needed by Pool converters

# get a handle on the application manager
from AthenaCommon.AppMgr import theApp

# get a handle on the Service manager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# we need the particle property service
if not hasattr(svcMgr, 'PartPropSvc'):
  from PartPropSvc.PartPropSvcConf import PartPropSvc
  svcMgr += PartPropSvc()
  pass

# we need the TruthParticleCnvTool
if not "McParticleTools" in theApp.Dlls:
  theApp.Dlls += ["McParticleTools"]

#
# Load needed POOL converters
include ("TrackParticleTruthAthenaPool/TrackParticleTruthAthenaPool_joboptions.py")
#

# For McEventCollection
if not "GeneratorObjectsAthenaPoolPoolCnv" in theApp.Dlls:
  include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

# For TruthParticleContainer
if not "McParticleEventAthenaPoolPoolCnv" in theApp.Dlls:
  include( "McParticleEventAthenaPool/McParticleEventAthenaPool_joboptions.py" )
  pass
