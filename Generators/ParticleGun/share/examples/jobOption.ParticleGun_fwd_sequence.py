#! -*- python -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

import ParticleGun as PG
pg = PG.ParticleGun()
pg.randomSeed = 123456
pg.sampler.pid = (2112, 22, 2112, 22)
pg.sampler.mom = PG.EThetaMPhiSampler(energy=(1360000, 500000, 1360000, 500000),
                                      theta=(0, 0, PG.PI, PG.PI))
pg.sampler.pos = PG.PosSampler(x=[-120,-100], y=[-10,10], z=203950)
topSeq += pg

include("GeneratorUtils/postJO.CopyWeights.py")
include("GeneratorUtils/postJO.PoolOutput.py")
include("GeneratorUtils/postJO.DumpMC.py")
