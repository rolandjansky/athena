# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigEFDiMuFex,TrigEFDiMuFex_xAOD
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

#from AthenaCommon.SystemOfUnits import GeV

class TrigEFDiMuFexConfig (TrigEFDiMuFex_xAOD):
    __slots__ = []

# the ver to use with no input
#    def __init__(self, name = "TrigEFDiMuFexConfig"):
#        super( TrigEFDiMuFex, self ).__init__( name )

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigEFDiMuFexConfig, cls ).__new__( cls, *newargs, **kwargs )
     
    def __init__( self, name, *args, **kwargs ):
        super( TrigEFDiMuFexConfig, self ).__init__( name )

        from TrigBphysHypo.TrigEFDiMuFexMonitoring import TrigEFDiMuFexValidationMonitoring
        validation = TrigEFDiMuFexValidationMonitoring()
        from TrigBphysHypo.TrigEFDiMuFexMonitoring import TrigEFDiMuFexOnlineMonitoring
        online = TrigEFDiMuFexOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        self.AthenaMonTools = [ validation, online, time ]

