#! -*- python -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

import ParticleGun as PG

class MyParticleSampler(PG.ParticleSampler):
    """
    A special sampler to generate single particles flat in 1/pT and in
    impact parameter to the beam, with flat z0.
    """

    def __init__(self):
        psamp = PG.PtEtaMPhiSampler(pt=PG.InvSampler(4000, 400000), eta=[0.1,0.3], phi=[0.3, 0.5])
        xsamp = PG.PosSampler(0, 0, [-150,150], 0)
        PG.ParticleSampler.__init__(self, pid={13,-13}, mom=psamp, pos=xsamp)
        self.ip = PG.mksampler([-2,2])

    def shoot(self):
        "Return a vector of sampled particles"
        ps = PG.ParticleSampler.shoot(self)
        assert len(ps) == 1
        p = ps[0]
        from math import sqrt
        m = -p.mom.X() / p.mom.Y() #< gradient of azimuthal IP sampling line, perp to mom
        x = self.ip() / sqrt(1 + m**2) #< just decomposing sampled IP into x component...
        y = m*x #< ... and y-component
        p.pos.SetX(x)
        p.pos.SetY(m*x)
        return [p]

topSeq += PG.ParticleGun()
topSeq.ParticleGun.randomSeed = 123456
topSeq.ParticleGun.sampler = MyParticleSampler()

include("GeneratorUtils/postJO.CopyWeights.py")
include("GeneratorUtils/postJO.PoolOutput.py")
include("GeneratorUtils/postJO.DumpMC.py")
