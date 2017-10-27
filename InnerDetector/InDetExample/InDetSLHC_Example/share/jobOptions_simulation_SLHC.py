#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Simulation
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-MC15c-SDR-06"


include("InDetSLHC_Example/preInclude.SLHC.py")

include("InDetSLHC_Example/preInclude.SiliconOnly.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput="singleMuon_test_slhc_Hits.pool.root"
athenaCommonFlags.EvtMax=10

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
#simFlags.SimLayout='ATLAS-SLHC-00-00-00'
#simFlags.SimLayout='ATLAS-SLHC-01-00-00_VALIDATION'
#simFlags.SimLayout='ATLAS-SLHC-01-03-00_VALIDATION'
simFlags.SimLayout='ATLAS-P2-ITK-01-00-00_VALIDATION'
simFlags.SimLayout.set_On() 
simFlags.EventFilter.set_On()
simFlags.RunNumber=240000

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Run ParticleGenerator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 13
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=10000, eta=[-3,3])
topSeq += pg

try:
    include("G4AtlasApps/fragment.SimCopyWeights.py")
except:
    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
    topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)


include("InDetSLHC_Example/postInclude.SLHC_Setup.py")

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# Output level & events
MessageSvc.OutputLevel      = 3
MessageSvc.defaultLimit=1000000;
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
