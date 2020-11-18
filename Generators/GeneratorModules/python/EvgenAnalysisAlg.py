# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaPython import PyAthena
import McParticleEvent.Pythonizations  # noqa: F401


class EvgenAnalysisAlg(PyAthena.Alg):
    """\
    Base class for implementing event generator interfaces in Python.
    Author: Andy Buckley
    """

    def __init__(self, name="EvgenAnalysisAlg"):
        super(EvgenAnalysisAlg, self).__init__(name=name)
        self.McEventKey = "GEN_EVENT"

    def events(self):
        "Get the current MC event collection"
        return self.evtStore[self.McEventKey]

    def event(self):
        "Get the first event in the collection"
        return self.events()[0]

    # TODO: add histo helpers cf. C++ GenAnalysisAlg
