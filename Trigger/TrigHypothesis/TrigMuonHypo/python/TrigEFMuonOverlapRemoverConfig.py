# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMuonHypo.TrigMuonHypoConf import *
from TrigMuonHypo.TrigEFMuonOverlapRemoverMonitoring import *

class TrigEFMuonOverlapRemoverConfig ( TrigEFMuonOverlapRemover ) :

    __slots__ = []

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
        return super( TrigEFMuonOverlapRemoverConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigEFMuonOverlapRemoverConfig, self ).__init__( name )

        # initial values
        self.DoMuExtrBasedRemoval = False

        # get args
        if len(args) != 2:
            raise Exception('TrigEFMuonOverlapRemover Misconfigured (args ne 2)')

        basedFex = args[0]
        cutSet   = args[1]

        if (basedFex=='MuExtr'):
            self.DoMuExtrBasedRemoval = True
        else:
            raise Exception('TrigEFMuonOverlapRemover Misconfigured (basedFex)!')
        
        # monitoring
        if (basedFex=='MuExtr'):
            validation = TrigEFMuonOverlapRemoverValidationMonitoringMuExtr()
            online     = TrigEFMuonOverlapRemoverOnlineMonitoringMuExtr()
            cosmic     = TrigEFMuonOverlapRemoverCosmicMonitoringMuExtr()
            self.AthenaMonTools = [ validation, online, cosmic ] 
    
        # cut defintion
        if (cutSet=='loose'):
            if(self.DoMuExtrBasedRemoval):
                self.MuExtrRequireDR       = True
                self.MuExtrRequireMass     = True
                self.MuExtrRequireSameSign = True
                self.MuExtrEtaBins         = [0, 9.9]
                self.MuExtrDRThres         = [0.0001]
                self.MuExtrMassThres       = [0.0001]
#        elif(cutSet=='nominal'):                
#            if(self.DoMuExtrBasedRemoval):
#                self.MuExtrRequireDR       = True
#                self.MuExtrRequireMass     = True
#                self.MuExtrRequireSameSign = True
#                self.MuExtrEtaBins         = [0, 0.9, 1.1, 1.9, 2.1, 9.9]
#                self.MuExtrDRThres         = [0.002, 0.001, 0.002, 0.002, 0.002]
#                self.MuExtrMassThres       = [0.004, 0.002, 0.006, 0.006, 0.006]
        elif(cutSet=='test'):
            if(self.DoMuExtrBasedRemoval):
                self.MuExtrRequireDR       = True
                self.MuExtrRequireMass     = False
                self.MuExtrRequireSameSign = False
                self.MuExtrEtaBins         = [0, 9.9]
                self.MuExtrDRThres         = [3.2]
                self.MuExtrMassThres       = [100]
        else:
            raise Exception('TrigEFMuonOverlapRemover Misconfigured (cutSet)!')
