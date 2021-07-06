#! -*- python -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

import ParticleGun as PG

class MyParticleSampler(PG.ParticleSampler):
    "A special sampler with two _correlated_ particles."

    def __init__(self):
        self.mom1 = PG.PtEtaMPhiSampler(pt=25000, eta=[-2,2])

    def shoot(self):
        "Return a vector of sampled particles"
        p1 = PG.SampledParticle(11, self.mom1.shoot())
        eta1 = p1.mom.Eta()
        phi1 = p1.mom.Phi()
        # TODO: will phi be properly wrapped into range?
        mom2 = PG.PtEtaMPhiSampler(pt=25000,
                                   eta=[eta1-0.5, eta1+0.5],
                                   phi=[phi1-0.5, phi1+0.5])
        p2 = PG.SampledParticle(11, mom2.shoot())
        return [p1, p2]

topSeq += PG.ParticleGun()
topSeq.ParticleGun.randomSeed = 123456
topSeq.ParticleGun.sampler = MyParticleSampler()

include("GeneratorUtils/postJO.CopyWeights.py")
include("GeneratorUtils/postJO.PoolOutput.py")
include("GeneratorUtils/postJO.DumpMC.py")
