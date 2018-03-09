#--------------------------------------------------------------
# Template jobOptions: ID only simulation for run 2 ATLAS
#  - Simulation
#--------------------------------------------------------------
## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag='OFLCOND-RUN12-SDR-19'


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput="singleMuon_R2_Hits.pool.root"
athenaCommonFlags.EvtMax=10

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()

simFlags.SimLayout.set_On()
simFlags.SimLayout='ATLAS-R2-2015-03-01-00'
simFlags.EventFilter.set_On()
simFlags.RunNumber = 222525

## Run ParticleGenerator
athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob

import ParticleGun as PG
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 13
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=1000, eta=[-3,3])
topSeq += pg

try:
    include("G4AtlasApps/fragment.SimCopyWeights.py")
except:
    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
    topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add G4 sim framework alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)


MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit     = 1000000
MessageSvc.Format           = "% F%37W%S%7W%R%T %0W%M"
