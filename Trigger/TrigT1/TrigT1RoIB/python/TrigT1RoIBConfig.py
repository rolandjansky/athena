
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigT1RoIB.TrigT1RoIBConf import ROIB__RoIBuilder

class DefaultRoIBuilder(ROIB__RoIBuilder):
    __slots__ = []
    
    def __init__(self, name = "DefaultRoIBuilder"):
        super( DefaultRoIBuilder, self ).__init__( name )
        self.CaloEMTauLocation = [ "CaloTriggerDataLocation/EmTauSlink0", 
                                   "CaloTriggerDataLocation/EmTauSlink1", 
                                   "CaloTriggerDataLocation/EmTauSlink2", 
                                   "CaloTriggerDataLocationEmTauSlink3" ]

        self.CaloJetEnergyLocation = [ "CaloTriggerDataLocation/JEPSlink0", 
                                       "CaloTriggerDataLocation/JEPSlink0" ]

    def setDefaults(self, handle):
        pass

class RoIBuilder(DefaultRoIBuilder):
    __slots__ = []

    def __init__(self, name = "RoIBuilder"):
        super( RoIBuilder, self ).__init__( name )

    def setDefaults(self, handle):
        pass

# algorithm configuration for running in a digitization job
class RoIBuilderInDigi(DefaultRoIBuilder):
    __slots__ = []

    def __init__(self, name = "RoIBuilder"):
        super( RoIBuilderInDigi, self ).__init__( name )
        
    def setDefaults(self, handle):
        DefaultRoIBuilder.setDefaults(handle)

        # switch off reading of Muon/Calo inputs if subsystem is not running
        from AthenaCommon.DetFlags import DetFlags
        handle.DoCalo = DetFlags.simulateLVL1.Calo_on()
        handle.DoMuon = DetFlags.simulateLVL1.Muon_on()




