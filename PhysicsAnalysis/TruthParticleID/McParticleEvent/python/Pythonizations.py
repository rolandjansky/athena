# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: McParticleEvent/python/Pythonizations.py
# @purpose: pythonizations of HepMC::GenEvent
# @author: Sebastien Binet <binet@cern.ch>

__version__ = "$Revision: 1.1 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

def _setup():
    from AthenaCommon.Logging import logging
    msg = logging.getLogger("McParticleEvent")

    import AthenaPython.PyAthena as PyAthena
    from PyDumper.Dumpers import toiter

    #######################
    msg.info("fixing up HepMC.GenEvent.particles iterator...")
    @property
    def particles(self):
        """lazy iterator over particles of a HepMC::GenEvent"""
        for p in toiter(self.particles_begin(),
                        self.particles_end()):
            yield p
    PyAthena.HepMC.GenEvent.particles = particles
    del particles

    # now, for vertices...
    msg.info("fixing up HepMC.GenEvent.vertices iterator...")
    @property
    def vertices(self):
        """lazy iterator over vertices of a HepMC::GenEvent"""
        for v in toiter(self.vertices_begin(),
                        self.vertices_end()):
            yield v
    PyAthena.HepMC.GenEvent.vertices = vertices
    del vertices

    # a nicer print method
    msg.info("fixing up HepMC.GenEvent.print method...")
    def __py_str__(self):
        """python version of HepMC::GenEvent::print"""
        s = PyAthena.std.stringstream()
        getattr(self, 'print')(s)
        return s.str()
    PyAthena.HepMC.GenEvent.__str__ = __py_str__
    del __py_str__
    #######################

    #######################
    msg.info("fixing up HepMC.GenVertex.particles_in iterator...")
    @property
    def particles_in(self):
        """lazy iterator over in-going particles of a vertex"""
        for p in toiter(self.particles_in_const_begin(),
                        self.particles_in_const_end()):
            yield p
    PyAthena.HepMC.GenVertex.particles_in = particles_in
    del particles_in
    
    msg.info("fixing up HepMC.GenVertex.particles_out iterator...")
    @property
    def particles_out(self):
        """lazy iterator over out-going particles of a vertex"""
        for p in toiter(self.particles_out_const_begin(),
                        self.particles_out_const_end()):
            yield p
    PyAthena.HepMC.GenVertex.particles_out = particles_out
    del particles_out

    # a nicer print method
    msg.info("fixing up HepMC.GenVertex.print method...")
    def __py_str__(self):
        """python version of HepMC::GenVertex::print"""
        s = PyAthena.std.stringstream()
        getattr(self, 'print')(s)
        return s.str()
    PyAthena.HepMC.GenVertex.__str__ = __py_str__
    del __py_str__
    #######################
    
    # a nicer print method
    msg.info("fixing up HepMC.GenParticle.print method...")
    def __py_str__(self):
        """python version of HepMC::GenParticle::print"""
        s = PyAthena.std.stringstream()
        getattr(self, 'print')(s)
        return s.str()
    PyAthena.HepMC.GenParticle.__str__ = __py_str__
    del __py_str__

    return

# setup at first import
_setup()
del _setup

