#! -*- python -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

import ParticleGun as PG
pg = PG.ParticleGun()
pg.randomSeed = 123456
pg.sampler.pid = {11,-11,211,111}
pg.sampler.mom = PG.EEtaMPhiSampler(energy=10000, eta=[-2,2])
topSeq += pg

include("GeneratorUtils/postJO.CopyWeights.py")
include("GeneratorUtils/postJO.PoolOutput.py")
include("GeneratorUtils/postJO.DumpMC.py")
