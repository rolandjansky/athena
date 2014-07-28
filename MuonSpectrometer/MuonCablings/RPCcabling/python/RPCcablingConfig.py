# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RPCcabling.RPCcablingConf import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

class RPCcablingConfig (RPCcablingSvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(RPCcablingConfig ,self).__init__(name)

    
    def setDefaults(cls,handle):
        if hasattr(handle,'ConfFileName'):
            if handle.ConfFileName!="RPCcabling.LVL1confAtlas.data":
                print "RPCcablingSvc uses a non standard configuration: ", handle.ConfFileName

        if hasattr(handle,'CorrFileName'):    
            if handle.CorrFileName!="RPCcabling.LVL1confAtlas.corr":
                print "RPCcablingSvc uses a non standard correction set: ", handle.CorrFileName



RPCcablingSvc = RPCcablingConfig()
ServiceMgr += RPCcablingSvc
