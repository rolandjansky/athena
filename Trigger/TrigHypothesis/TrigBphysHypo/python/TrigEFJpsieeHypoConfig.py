# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFJpsieeHypo

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
#from AthenaCommon.SystemOfUnits import GeV

# basic cut
class EFJpsieeHypo_1 (TrigEFJpsieeHypo):
    __slots__ = []
    def __init__(self, name = "EFJpsieeHypo_1"):
        super( TrigEFJpsieeHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts

#        from TrigBphysHypo.TrigEFJpsieeHypoMonitoring import TrigEFJpsieeHypoValidationMonitoring
#        validation = TrigEFJpsieeHypoValidationMonitoring()
        
#        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
#        time = TrigTimeHistToolConfig("Time")


# basic cut
class EFJpsieeHypo_FS (TrigEFJpsieeHypo):
    __slots__ = []
    def __init__(self, name = "EFJpsieeHypo_FS"):
        super( TrigEFJpsieeHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False

        # L2 Jpsiee cuts
 
 #       from TrigBphysHypo.TrigEFJpsieeHypoMonitoring import TrigEFJpsieeHypoValidationMonitoring
 #       validation = TrigEFJpsieeHypoValidationMonitoring()
        
 #       from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
 #       time = TrigTimeHistToolConfig("Time")

