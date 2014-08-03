# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class _AnalysisFlags:

    # Particle Analysis Flags Default is ON=True
    _partAna = [
        'DoPhoton',
        'DoElectron',
        'DoMuon',
        'DoTauJet',
        'DoParticleJet',
        'DoBJet',
        'DoNavigation',
        'DoInteractive'        
        ]
    
    def __init__ (self):
        # Particle Analysis Attributes
        for attr in self._partAna:
            setattr(self, attr, True)
        self.DoNavigation = False
        self.DoInteractive = False
    def Print (self):
        format = "%17s : %s"
        for attr in self._partAna:
            if getattr(self, attr):
                print format % (attr, "ON")
            else:
                print format % (attr, "--")

AnalysisFlags = _AnalysisFlags()


