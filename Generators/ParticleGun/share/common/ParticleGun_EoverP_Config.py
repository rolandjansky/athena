#! -*- python -*-
evgenConfig.description = "Single particle gun for E/p event generation"
evgenConfig.keywords = ["singleParticle",]
evgenConfig.generators = ["ParticleGun"]
evgenConfig.contact = ["zach.marshall@cern.ch"]

import ParticleGun as PG
import ROOT
from ParticleGun.samplers import *
class PEtaSampler(PG.MomSampler):
    "Create a 4-momentum vector from pt, eta, m and phi distributions/samplers."

    def __init__(self, momentum, eta, pid=211, phi=[0, math.pi*2.]):
        self.momentum = momentum
        self.eta = eta
        pdg_table = ROOT.TDatabasePDG.Instance()
        mass = pdg_table.GetParticle(pid).Mass()
        self.mass = mass
        self.phi = phi

    @property
    def momentum(self):
        "Momentum sampler"
        return self._momentum
    @momentum.setter
    def momentum(self, x):
        self._momentum = mksampler(x)

    @property
    def eta(self):
        "Pseudorapidity sampler"
        return self._eta
    @eta.setter
    def eta(self, x):
        self._eta = mksampler(x)

    @property
    def mass(self):
        "Mass sampler"
        return self._m
    @mass.setter
    def mass(self, x):
        self._m = mksampler(x)

    @property
    def phi(self):
        "Azimuthal angle sampler"
        return self._phi
    @phi.setter
    def phi(self, x):
        self._phi = mksampler(x)

    def shoot(self):
        v4 = ROOT.TLorentzVector()
        pt = p / math.cosh(self.eta())
        v4.SetPtEtaPhiM(pt, self.eta(), self.phi(), self.mass())
        return v4

a_particle = int(jofile.split('_')[-1].split('.py')[0].replace('m','-'))

pg = PG.ParticleGun()
pg.sampler.pid = int(a_particle) #PID
pg.sampler.mom = PEtaSampler(momentum=(500,800,1000,1200,1500,2000,3000,4000,5000,6000,7000,8000,9000,10000,11000,13000,15000,17000,20000,\
                                       25000,35000,50000,75000,100000,200000,350000,500000), eta=[-0.3,0.3], pid=int(a_particle))
genSeq += pg

