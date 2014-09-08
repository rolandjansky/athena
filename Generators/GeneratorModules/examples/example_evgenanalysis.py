# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

include("GeneratorUtils/StdAnalysisSetup.py")

class MyAna(EvgenAnalysisAlg):

    def __init__(self, name="MyAna"):
        super(MyAna, self).__init__(name=name)

    def execute(self):
        evtcoll = self.evtStore[self.McEventKey]
        evt = evtcoll[0]
        #evt.weights().push_back(3.142)
        print evt.particles_size(), ":", [w for w in evt.weights()]
        return StatusCode.Success


svcMgr.EventSelector.InputCollections = ["mc.evnt.pool.root"]
topSeq += MyAna()
