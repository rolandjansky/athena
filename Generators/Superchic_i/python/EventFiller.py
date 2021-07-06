# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GeneratorModules.EvgenAlg import EvgenAlg
from AthenaPython.PyAthena import HepMC, StatusCode
#import McParticleEvent.Pythonizations
import os
import ROOT
#import random


class LheEVNTFiller(EvgenAlg):

    def __init__(self, name="LheEVNTFiller"):
        super(LheEVNTFiller, self).__init__(name=name)

    fileName = "evrecs/evrecout.dat"
    eventsProcessed = 0

    def initialize(self):
#        seed = None
        if(os.path.isfile(self.fileName)):
            print(self.fileName)
            return StatusCode.Success
        else:
            return StatusCode.Failure


    def fillEvent(self, evt):

        eventsSeen = 0
        firstLine = True
#        particlesSeen = 0

        with open(self.fileName,'r') as inputfile:
            event = False
            for line in inputfile:
                if not event and '<event>' not in line:
                    continue
            # Check if we are just starting an event
                if not event and '<event>' in line and eventsSeen == self.eventsProcessed:
                    event = True
                    continue
            # Check if we have finished and are doing something else
                if '<' in line or '>' in line:
                    event = False
                    eventsSeen += 1
                    firstLine = True
                    continue
                if event and firstLine:
                    firstLine = False
                    evt.weights().push_back(float(line.split()[2]))
                    continue
                if event:
                    pos = HepMC.FourVector(0.0, 0.0, 0.0, 0.0)
                    gv = HepMC.GenVertex(pos)
                    ROOT.SetOwnership(gv, False)
                    evt.add_vertex(gv)
                    mom = HepMC.FourVector( float(line.split()[6])*1000. , float(line.split()[7])*1000. , float(line.split()[8])*1000. , float(line.split()[9])*1000. )
                    gp = HepMC.GenParticle()
                    gp.set_status(int(line.split()[1]) )
                    gp.set_pdg_id(int(line.split()[0]) )
                    gp.set_momentum(mom)
                    print(float(line.split()[10]) * 1000.)
                    gp.set_generated_mass(float(line.split()[10]) * 1000.)
                    ROOT.SetOwnership(gp, False)
                    gv.add_particle_out(gp)
                    
        self.eventsProcessed += 1

        return StatusCode.Success

