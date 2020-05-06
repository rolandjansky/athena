#! -*- python -*-
import math
import ROOT

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 5000

import ParticleGun as PG
      
class MyParticleSampler(PG.ParticleSampler):

    "A special sampler to shoot in the HEC wheel 1 or 2"
	
    def __init__(self, pid = -11,
                 energy = 100000.,
                 phi = PG.UniformSampler(0, 2*math.pi),
                 eta = PG.UniformSampler(1.5,3.3),
                 pv  = PG.PosSampler(0, 0, 4319.5)):
        self.pid = pid
        self.energy = energy
        self.eta = eta
        self.phi = phi
        self.pv =  pv
          
    def shoot(self):
        mass = 0.510998928
        momentum = math.sqrt(self.energy**2 - mass**2)
        v4eta = self.eta.shoot()
        v4 = ROOT.TLorentzVector()
        v4.SetPtEtaPhiE(momentum/math.cosh(v4eta), v4eta, self.phi.shoot(), self.energy)
        vv = self.pv.shoot()
        v4theta = v4.Theta()
        v4phi = v4.Phi()
        z = vv.Z()
        ## Calculate r at z
        r = math.tan(v4theta)*z
        x = r*math.cos(v4phi)
        y = r*math.sin(v4phi)
        vv.SetX(x)
        vv.SetY(y)
        pdg_id = self.pid.shoot()

        print "pdg_id = ",pdg_id
        rtn = [PG.SampledParticle(pdg_id,v4,vv)]

        return rtn


topSeq += PG.ParticleGun()
topSeq.ParticleGun.sampler = MyParticleSampler()

include("GeneratorUtils/postJO.WriteHepMC.py")
include("GeneratorUtils/postJO.Output.py")

print topSeq


