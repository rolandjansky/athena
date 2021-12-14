# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GeneratorModules.EvgenAlg import EvgenAlg
from ParticleGun.samplers import ParticleSampler
from ParticleGun.samplers import * # noqa: F401, F403 (import into our namespace)
# commenting out the HepMC import for now
#try:
#          from AthenaPython.PyAthena import HepMC3  as HepMC
#except ImportError:
#          from AthenaPython.PyAthena import HepMC   as HepMC  

from AthenaPython.PyAthena import StatusCode
import ROOT,random

__author__ = "Andy Buckley <andy.buckley@cern.ch>"

class ParticleGun(EvgenAlg):
    """
    A simple but flexible algorithm for generating events from simple distributions.
    """

    def __init__(self, name="ParticleGun", randomSvcName="AtRndmGenSvc", randomStream="ParticleGun", randomSeed=None):
        super(ParticleGun, self).__init__(name=name)
        self.samplers = [ParticleSampler()]
        self.randomStream = randomStream
        self.randomSvcName = randomSvcName
        self.randomSeed = randomSeed

    @property
    def sampler(self):
        "Get the first (and presumed only) sampler"
        return self.samplers[0] if self.samplers else None
    @sampler.setter
    def sampler(self, s):
        "Set the samplers list to include only a single sampler, s"
        self.samplers = [s]


    def initialize(self):
        """
        Pass the AtRndmGenSvc seed to Python's random module, or use a fixed value set via pg.randomSeed.
        """
        seed = None
        ## Use self.randomSeed directly, or if it's None find a seed string from the ATLAS random number service
        if self.randomSeed is not None:
            seed = self.randomSeed
        else:
            randomSvc = getattr(svcMgr, self.randomSvcName, None)
            if randomSvc is not None:
                for seedstr in randomSvc.Seeds:
                    if seedstr.startswith(self.randomStream):
                        seed = seedstr
                        self.msg.info("ParticleGun: Using random seed '%s' ", seed)
                        break
                if seed is None:
                    self.msg.warning("ParticleGun: Failed to find a seed for the random stream named '%s' ", self.randomStream)
            else:
                self.msg.warning("ParticleGun: Failed to find random number service called '%s' ", self.randomSvcName)
        ## Apply the seed
        if seed is not None:
            random.seed(seed)
            return StatusCode.Success
        else:
            self.msg.error("ParticleGun: randomSeed property not set, and no %s random number service found", self.randomSvcName)
            return StatusCode.Failure


    def fillEvent(self, evt):
        """
        Sample a list of particle properties, which are then used to create a new GenEvent in StoreGate.
        """
        ## Set event weight(s)
        # TODO: allow weighted sampling?
        try:
          from AthenaPython.PyAthena import HepMC3  as HepMC
        except ImportError:
          from AthenaPython.PyAthena import HepMC   as HepMC
        evt.weights().push_back(1.0)

        ## Make and fill particles
        for s in self.samplers:
            particles = s.shoot()
            for p in particles:
                ## Debug printout of particle properties
                #print DEBUG0, p.pid, p.mom.E(), p.mom.Pt(), p.mom.M()
                #print "DEBUG1 (px,py,pz,E) = (%0.2e, %0.2e, %0.2e, %0.2e)" % (p.mom.Px(), p.mom.Py(), p.mom.Pz(), p.mom.E())
                #print "DEBUG2 (eta,phi,pt,m) = (%0.2e, %0.2e, %0.2e, %0.2e)" % (p.mom.Eta(), p.mom.Phi(), p.mom.Pt(), p.mom.M())
                #print "DEBUG3 (x,y,z,t) = (%0.2e, %0.2e, %0.2e, %0.2e)" % (p.pos.X(), p.pos.Y(), p.pos.Z(), p.pos.T())

                ## Make particle-creation vertex
                # TODO: do something cleverer than one vertex per particle?
                pos = HepMC.FourVector(p.pos.X(), p.pos.Y(), p.pos.Z(), p.pos.T())
                gv = HepMC.GenVertex(pos)
                ROOT.SetOwnership(gv, False)
                evt.add_vertex(gv)

                ## Make particle with status == 1
                mom = HepMC.FourVector(p.mom.Px(), p.mom.Py(), p.mom.Pz(), p.mom.E())
                gp = HepMC.GenParticle()
                gp.set_status(1)
                gp.set_pdg_id(p.pid)
                gp.set_momentum(mom)
                if p.mass is not None:
                    gp.set_generated_mass(p.mass)
                ROOT.SetOwnership(gp, False)
                gv.add_particle_out(gp)

        return StatusCode.Success


## PyAthena HepMC notes
#
## evt.print() isn't valid syntax in Python2 due to reserved word
# TODO: Add a Pythonisation, e.g. evt.py_print()?
#getattr(evt, 'print')()
#
## How to check that the StoreGate key exists and is an McEventCollection
# if self.sg.contains(McEventCollection, self.sgkey):
#     print self.sgkey + " found!"
#
## Modifying an event other than that supplied as an arg
# mcevts = self.sg[self.sgkey]
# for vtx in mcevts[0].vertices: # only way to get the first vtx?!
#     gp2 = HepMC.GenParticle()
#     gp2.set_momentum(HepMC.FourVector(1,2,3,4))
#     gp2.set_status(1)
#     vtx.add_particle_out(gp2)
#     break
