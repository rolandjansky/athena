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

        if self.evtStore.contains(McEventCollection, self.McEventKey):
            self.msg.debug(self.McEventKey + " found before alg execution!")

        ## Retrieve MC event collection or create a new one
        mcevts = None
        try:
            mcevts = self.evtStore[self.McEventKey]
        except:
            mcevts = McEventCollection()
            try:
                self.evtStore.record(mcevts, self.McEventKey, True, False)
            except:
                self.evtStore.record(mcevts, self.McEventKey)
        ROOT.SetOwnership(mcevts, False)

        if self.evtStore.contains(McEventCollection, self.McEventKey):
            self.msg.debug(self.McEventKey + " found after alg execution!")

        ## Get the first event from the MCEC, or make a new one
        evt = None
        try:
            evt = mcevts[0]
        except:
            evt = HepMC.GenEvent()
            mcevts.push_back(evt)
        ROOT.SetOwnership(evt, False)

        ## Fill/modify the event
        return self.fillEvent(evt)


    def finalize(self):
        return self.genFinalize()
