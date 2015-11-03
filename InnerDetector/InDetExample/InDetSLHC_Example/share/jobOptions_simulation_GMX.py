#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Simulation
#--------------------------------------------------------------

execfile("ID_only.py")
execfile("geometry.py")

import random
if 'randomSeedPhrase' not in dir() :
    randomSeedPhrase = "youSexyThing"
random.seed(randomSeedPhrase)
SimFlags.RandomSeedList.addSeed('VERTEX',  random.randint(10000, 99999999), random.randint(10000, 99999999))
SimFlags.RandomSeedList.addSeed('AtlasG4', random.randint(10000, 99999999), random.randint(10000, 99999999))
SimFlags.RandomSeedList.addSeed('SINGLE',  random.randint(10000, 99999999), random.randint(10000, 99999999))

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = "singleMuon_" + randomSeedPhrase + "_slhc_Hits.pool.root"
athenaCommonFlags.EvtMax = 10000

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Run ParticleGenerator
athenaCommonFlags.PoolEvgenInput.set_Off()
import AthenaCommon.AtlasUnixGeneratorJob
import ParticleGun as PG

from G4AtlasApps.SimFlags import SimFlags
# The following will work in later releases than 20.0 (From Nick) for breaking up jobs: athena -c 'myRandomSeed = 777'
#if 'myRandomSeed' not in dir() :
#    myRandomSeed = 12398190
#pg = PG.ParticleGun(randomSvcName=SimFlags.RandomSvc.get_Value(), randomStream="SINGLE", randomSeed=myRandomSeed)
pg = PG.ParticleGun(randomSvcName=SimFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = 13
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=100000, eta=[-2.7, 2.7])

topSeq += pg
#try:
#    from EvgenProdTools.EvgenProdToolsConf import CopyEventWeight
#except:
#    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
#topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

# Output
MessageSvc.defaultLimit = 500
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
