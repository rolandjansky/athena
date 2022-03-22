printfunc ('SINGLE PARTICLE GENERATOR')

## Run ParticleGun
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

import ParticleGun as PG
from G4AtlasApps.SimFlags import simFlags
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = PG.CyclicSeqSampler([-11,11])
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=10000, eta=[-3,3])
topSeq += pg

include("G4AtlasApps/fragment.SimCopyWeights.py")
