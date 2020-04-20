# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Track Slimming 
#
#==============================================================


from GaudiPython import PyAlgorithm

class slimTracks( PyAlgorithm ):
    def __init__ ( self, name ) :
        PyAlgorithm.__init__(self,name)
        self.name = name

    def initialize(self):
        printfunc ("Initializing filterAlg")
        return True

    def execute(self):
        trs = PyParticleTools.getTrackParticles("TrackParticleCandidate")
        for tr in trs:
            if tr.pt()<5*GeV:
                tr.removeSummary()
                tr.removeErrorMatrix()
        return True

    def finalize(self):
        return True

    
