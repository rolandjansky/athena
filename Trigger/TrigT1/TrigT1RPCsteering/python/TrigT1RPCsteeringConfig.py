# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigT1RPCsteering.TrigT1RPCsteeringConf import TrigT1RPC
from AthenaCommon.Configurable import Configurable
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AlgSequence import AlgSequence

class TrigT1RPCsteeringConfig (TrigT1RPC):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(TrigT1RPCsteeringConfig ,self).__init__(name)

    def setDefaults(cls,handle):
        if 'TrigT1RPChardware' not in theApp.Dlls:
            print ("loading TrigT1RPChardware library")
            theApp.Dlls += [ "TrigT1RPChardware" ]

        if 'TrigT1RPClogic' not in theApp.Dlls:
            print ("loading TrigT1RPClogic library")
            theApp.Dlls += [ "TrigT1RPClogic" ]

job = AlgSequence()
TrigT1RPC = TrigT1RPCsteeringConfig()
job += TrigT1RPC
