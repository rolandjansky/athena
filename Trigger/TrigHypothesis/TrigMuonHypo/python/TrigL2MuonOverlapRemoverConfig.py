# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMuonHypo.TrigMuonHypoConf import *
from TrigMuonHypo.TrigL2MuonOverlapRemoverMonitoring import *

class TrigL2MuonOverlapRemoverConfig ( TrigL2MuonOverlapRemover ) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigL2MuonOverlapRemoverConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigL2MuonOverlapRemoverConfig, self ).__init__( name )

        # initial values
        self.DoMufastBasedRemoval = False
        self.DoMucombBasedRemoval = False

        # get args
        if len(args) != 2:
            raise Exception('TrigL2MuonOverlapRemover Misconfigured (args ne 2)')

        basedFex = args[0]
        cutSet   = args[1]

        if (basedFex=='Mufast'):
            self.DoMufastBasedRemoval = True
        elif (basedFex=='Mucomb'):
            self.DoMucombBasedRemoval = True
        else:
            raise Exception('TrigL2MuonOverlapRemover Misconfigured (basedFex)!')
        
        # monitoring
        if (basedFex=='Mufast'):
            validation = TrigL2MuonOverlapRemoverValidationMonitoringMufast()
            online     = TrigL2MuonOverlapRemoverOnlineMonitoringMufast()
            cosmic     = TrigL2MuonOverlapRemoverCosmicMonitoringMufast()
            self.AthenaMonTools = [ validation, online, cosmic ] 
        if (basedFex=='Mucomb'):
            validation = TrigL2MuonOverlapRemoverValidationMonitoringMucomb()
            online     = TrigL2MuonOverlapRemoverOnlineMonitoringMucomb()
            cosmic     = TrigL2MuonOverlapRemoverCosmicMonitoringMucomb()
            self.AthenaMonTools = [ validation, online, cosmic ] 
    
        # cut defintion
        if (cutSet=='loose'):
            if(self.DoMufastBasedRemoval):
                self.MufastRequireDR       = True
                self.MufastRequireMass     = True
                self.MufastRequireSameSign = True
                # BB
                self.MufastDRThresBB       = 0.0001
                self.MufastMassThresBB     = 0.0001
                # BE
                self.MufastDRThresBE       = 0.0001
                self.MufastMassThresBE     = 0.0001
                # EE
                self.MufastEtaBinsEC       = [0, 9.9]
                self.MufastDRThresEC       = [0.0001]
                self.MufastDRThresEC       = [0.0001]
            if(self.DoMucombBasedRemoval):
                self.MucombRequireDR       = True
                self.MucombRequireMufastDR = True
                self.MucombRequireMass     = True
                self.MucombRequireSameSign = True
                self.MucombEtaBins         = [0, 9.9]
                self.MucombDRThres         = [0.0001]
                self.MucombMufastDRThres   = [0.4]
                self.MucombMassThres       = [0.0001]
        elif(cutSet=='nominal'):                
            if(self.DoMufastBasedRemoval):
                self.MufastRequireDR       = True
                self.MufastRequireMass     = True
                self.MufastRequireSameSign = True
                # BB
                self.MufastDRThresBB       = 0.05
                self.MufastMassThresBB     = 0.20
                # BE
                self.MufastDRThresBE       = 0.05
                self.MufastMassThresBE     = 0.20
                # EE
                self.MufastEtaBinsEC       = [0, 1.9, 2.1, 9.9]
                self.MufastDRThresEC       = [0.06, 0.05, 0.05]
                self.MufastMassThresEC     = [0.20, 0.15, 0.10]
            if(self.DoMucombBasedRemoval):
                self.MucombRequireDR       = True
                self.MucombRequireMufastDR = True
                self.MucombRequireMass     = True
                self.MucombRequireSameSign = True
                self.MucombEtaBins         = [0, 0.9, 1.1, 1.9, 2.1, 9.9]
                self.MucombDRThres         = [0.002, 0.001, 0.002, 0.002, 0.002]
                self.MucombMufastDRThres   = [0.4,   0.4,   0.4,   0.4,   0.4]
                self.MucombMassThres       = [0.004, 0.002, 0.006, 0.006, 0.006]
        elif(cutSet=='test'):
            if(self.DoMufastBasedRemoval):
                self.MufastRequireDR       = False
                self.MufastRequireMass     = True
                self.MufastRequireSameSign = False
                # BB
                self.MufastDRThresBB       = 0.5
                self.MufastMassThresBB     = 75
                # BE
                self.MufastDRThresBE       = 0.5
                self.MufastMassThresBE     = 75
                # EE
                self.MufastEtaBinsEC       = [0, 9.9]
                self.MufastDRThresEC       = [0.5]
                self.MufastMassThresEC     = [75]
            if(self.DoMucombBasedRemoval):
                self.MucombRequireDR       = True
                self.MucombRequireMufastDR = True
                self.MucombRequireMass     = False
                self.MucombRequireSameSign = False
                self.MucombEtaBins         = [0, 9.9]
                self.MucombDRThres         = [3.2]
                self.MucombMufastDRThres   = [3.2]
                self.MucombMassThres       = [100]
        else:
            raise Exception('TrigL2MuonOverlapRemover Misconfigured (cutSet)!')
