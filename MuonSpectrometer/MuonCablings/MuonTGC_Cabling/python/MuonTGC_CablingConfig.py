# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonTGC_Cabling.MuonTGC_CablingConf import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

class MuonTGC_CablingConfig (MuonTGC_CablingSvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(MuonTGC_CablingConfig ,self).__init__(name)

TGCcablingSvc = MuonTGC_CablingConfig()
ServiceMgr += TGCcablingSvc

