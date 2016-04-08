# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class _AnalysisTriggerToolsFlags:

    _partAna = [
        'Release13AOD',
        'Release14AOD',
        'Release15AOD',
        'DoFDR'
        ]

    def __init__ (self):
        for attr in self._partAna:
            setattr(self, attr, True)
        self.Release13AOD = False
        self.Release15AOD = False
        self.DoFDR = False
    def Print (self):
        format = "%17s : %s"
        for attr in self._partAna:
            if getattr(self, attr):
                print format % (attr, "ON")
            else:
                print format % (attr, "--")

 
AnalysisTriggerToolsFlags = _AnalysisTriggerToolsFlags()
       
