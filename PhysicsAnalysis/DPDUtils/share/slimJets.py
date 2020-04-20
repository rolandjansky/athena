# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Jet Slimming 
#
#==============================================================


from GaudiPython import PyAlgorithm

class slimJets( PyAlgorithm ):
    def __init__ ( self, name ) :
        PyAlgorithm.__init__(self,name)
        self.name = name

    def initialize(self):
        printfunc ("Initializing filterAlg")
        return True

    def execute(self):
        jets = PyParticleTools.getJets("Cone4H1TowerJets")
        for jet in jets:
           jet.constituent("Tracks").removeAll() 
           for key in jet.infoKeys():
               if key != "IP3D" and key != "SV1":
                   jet.removeInfo(key)

        jets = PyParticleTools.getJets("Cone4TruthJets")
        for jet in jets:
           jet.constituent("Tracks").removeAll() 
           for key in jet.infoKeys():
               if key != "IP3D" and key != "SV1":
                   jet.removeInfo(key)

        return True

    def finalize(self):
        return True

    
