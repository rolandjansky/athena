#! -*- python -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

import ParticleGun as PG
pg = PG.ParticleGun()
pg.randomSeed = 123456
pg.sampler.pid = 13
pg.sampler.pos = PG.PosSampler(x=3140.0, y=[-154.134,154.134], z=[4938.76,5121.29], t=5929.7)
pg.sampler.mom = PG.EEtaMPhiSampler(energy=100000, eta=1.25, phi=0.0)
topSeq += pg

include("GeneratorUtils/postJO.CopyWeights.py")
include("GeneratorUtils/postJO.PoolOutput.py")
include("GeneratorUtils/postJO.DumpMC.py")
