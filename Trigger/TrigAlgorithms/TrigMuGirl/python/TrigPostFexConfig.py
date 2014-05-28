# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMuGirl.TrigMuGirlConf import TrigPostFex
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

#from AthenaCommon.SystemOfUnits import GeV

class TrigPostFexConfig (TrigPostFex):
    __slots__ = []

# the ver to use with no input
#    def __init__(self, name = "TrigPostFexConfig"):
#        super( TrigPostFex, self ).__init__( name )

    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
        return super( TrigPostFexConfig, cls ).__new__( cls, *newargs, **kwargs )
     
    def __init__( self, name, *args, **kwargs ):
        super( TrigPostFexConfig, self ).__init__( name )

#        from TrigMuGirl.TrigPostFexMonitoring import TrigPostFexValidationMonitoring
#        validation = TrigPostFexValidationMonitoring()
#        from TrigMuGirl.TrigPostFexMonitoring import TrigPostFexOnlineMonitoring
#        online = TrigPostFexOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
#        self.AthenaMonTools = [ validation, online, time ]
        self.AthenaMonTools = [ time ]

