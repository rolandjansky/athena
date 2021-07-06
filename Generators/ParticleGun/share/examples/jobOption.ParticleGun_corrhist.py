#! -*- python -*-

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## ROOT 2D histogram sampling alg (in ParticleGun.histsampling) by Andy Buckley
## Thanks to Alejandro Alonso for the initial Athena example on which this is based.

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

import ParticleGun as PG

class PtEtaHistParticleSampler(PG.ParticleSampler):
    "Particle sampler with correlated pT and eta from a 2D histogram."

    def __init__(self, pid, histfile, num=100):
        self.pid = PG.mksampler(pid)
        self.hist = PG.TH2(histfile, "h_pt_eta")
        self.numparticles = num

    def shoot(self):
        "Return a vector of sampled particles from the provided pT--eta histogram"
        particles = []
        for i in xrange(self.numparticles):
            ptrand, etarand = self.hist.GetRandom()
            ptrand *= 1000 # NB. This _particular_ histogram is in GeV, but Athena needs MeV!
            # TODO: Provide 4-mom construction functions to avoid building this one-time sampler
            pid = self.pid()
            mom = PG.PtEtaMPhiSampler(pt=ptrand, eta=etarand, mass=PG.MASSES[abs(pid)])
            p = PG.SampledParticle(pid, mom())
            #print p.mom.Pt(), "\t", p.mom.Eta(), "\t", p.mom.Phi(), "\t", p.mom.M()
            particles.append(p)
        return particles

topSeq += PG.ParticleGun()
topSeq.ParticleGun.randomSeed = 123456
topSeq.ParticleGun.sampler = PtEtaHistParticleSampler(11, "data_histos_el_1470pt.root")

include("GeneratorUtils/postJO.CopyWeights.py")
include("GeneratorUtils/postJO.PoolOutput.py")
include("GeneratorUtils/postJO.DumpMC.py")
