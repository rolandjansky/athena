# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonRPC_Cabling.MuonRPC_CablingConf import *

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

class MuonRPC_CablingConfig (MuonRPC_CablingSvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(MuonRPC_CablingConfig ,self).__init__(name)

    
    def setDefaults(cls,handle):
        if hasattr(handle,'ConfFileName'):
            if handle.ConfFileName!="LVL1confAtlas.data":
                print "RPCcablingSvc uses a non standard configuration: ", handle.ConfFileName

        if hasattr(handle,'CorrFileName'):    
            if handle.CorrFileName!="LVL1confAtlas.corr":
                print "RPCcablingSvc uses a non standard correction set: ", handle.CorrFileName


MuonRPC_CablingSvc = MuonRPC_CablingConfig()
ServiceMgr += MuonRPC_CablingSvc


