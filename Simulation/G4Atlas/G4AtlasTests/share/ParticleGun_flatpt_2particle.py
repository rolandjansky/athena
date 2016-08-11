#! -*- python -*-

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 10

import ParticleGun as PG
pg = PG.ParticleGun()
pg.randomSeed = 123456
pg.samplers.append(PG.ParticleSampler()) # add a second sampler
pg.samplers[0].pid = (-13, 13) # cycle mu+-
pg.samplers[0].mom = PG.PtEtaMPhiSampler(pt=[4000, 100000], eta=[1.0, 3.2]) # flat in pt and +ve eta
pg.samplers[1].pid = (13, -13) # cycle mu-+
pg.samplers[1].mom = PG.PtEtaMPhiSampler(pt=[4000, 100000], eta=[-3.2, -1.0]) # flat in pt and -ve eta
topSeq += pg

include("GeneratorUtils/postJO.CopyWeights.py")
include("GeneratorUtils/postJO.PoolOutput.py")
include("GeneratorUtils/postJO.DumpMC.py")
