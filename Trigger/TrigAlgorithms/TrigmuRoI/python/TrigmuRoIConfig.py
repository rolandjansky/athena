# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigmuRoI.TrigmuRoIConf import *
from TrigmuRoI.TrigmuRoIMonitoring import *
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

class TrigmuRoIConfig (TrigmuRoI):

    __slots__ = []
    
    #def __new__( cls, *args, **kwargs ):
    #    newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)
    #    return super( TrigmuFastConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name="TrigmuRoI" ):
        super( TrigmuRoIConfig, self ).__init__( name )

        validation = TrigmuRoIValidationMonitoring()
        online     = TrigmuRoIOnlineMonitoring()
        cosmic     = TrigmuRoICosmicMonitoring()

        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 5]
	#time.NumberOfHistBins = 50
	
        self.AthenaMonTools = [ validation, online, cosmic, time ]
