# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode, McEventCollection, HepMC, CLHEP
import McParticleEvent.Pythonizations
import ROOT


class EvgenAlg(PyAthena.Alg):
    """\
    Base class for implementing event generator interfaces in Python.
    Author: Andy Buckley
    """

    def __init__(self, name="EvgenAlg"):
        super(EvgenAlg, self).__init__(name=name)
        self.McEventKey = "GEN_EVENT"


    def genInitialize(self):
        return StatusCode.Success

    def fillEvent(self, evt):
        return StatusCode.Success

    def genFinalize(self):
        return StatusCode.Success


    def initialize(self):
        self.msg.debug("Initializing [%s]", self.name())
        return self.genInitialize()


    def execute(self):
        self.msg.debug("Executing [%s]", self.name())

        ## Retrieve MC event collection or create a new one
        mcevts = None
        if self.evtStore.contains(McEventCollection, self.McEventKey):
            self.msg.debug(self.McEventKey + " found before alg execution!")
            mcevts = self.evtStore[self.McEventKey]
        else:
            self.msg.debug("Creating " + self.McEventKey + " before alg execution!")
            mcevts = McEventCollection()
            self.evtStore.record(mcevts, self.McEventKey, True, False)
        ROOT.SetOwnership(mcevts, False)

        if self.evtStore.contains(McEventCollection, self.McEventKey):
            self.msg.debug(self.McEventKey + " found after alg execution!")

        ## Get the first event from the MCEC, or make a new one
        evt = None
        if mcevts.size() == 0:
            evt = HepMC.GenEvent()
            mcevts.push_back(evt)
        else:
            evt = mcevts[0]
        ROOT.SetOwnership(evt, False)

        ## Fill/modify the event
        st = self.fillEvent(evt)
        #print "FILLEVENT RESULT:"
        #getattr(evt, 'print')()
        return st


    def finalize(self):
        return self.genFinalize()
