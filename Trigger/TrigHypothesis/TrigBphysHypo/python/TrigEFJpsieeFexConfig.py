# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFJpsieeFex

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
#from AthenaCommon.SystemOfUnits import GeV

# basic cut
class EFJpsieeFex_1 (TrigEFJpsieeFex):
    __slots__ = []
    def __init__(self, name = "EFJpsieeFex_1"):
        super( TrigEFJpsieeFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts

#        from TrigBphysHypo.TrigEFJpsieeFexMonitoring import TrigEFJpsieeFexValidationMonitoring_RoI
#        validation = TrigEFJpsieeFexValidationMonitoring_RoI()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
                
#        self.AthenaMonTools = [ validation, time ]

# full scan
class EFJpsieeFex_FS (TrigEFJpsieeFex):
    __slots__ = []
    def __init__(self, name = "EFJpsieeFex_FS"):
        super( TrigEFJpsieeFex, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts

#        from TrigBphysHypo.TrigEFJpsieeFexMonitoring import TrigEFJpsieeFexValidationMonitoring_FS
#        validation = TrigEFJpsieeFexValidationMonitoring_FS()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")
                
#        self.AthenaMonTools = [ validation, time ]
