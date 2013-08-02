# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TGCcabling.TGCcablingConf import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

class TGCcablingConfig (TGCcablingSvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(TGCcablingConfig ,self).__init__(name)

        if ( globalflags.DetDescrVersion().startswith('DC1') or globalflags.DetDescrVersion().startswith('DC2') ):
            self.databaseASDToPP = "ASD2PP_P03.db"
            self.databaseInPP    = "PP_P03.db"
        
TGCcablingSvc = TGCcablingConfig()
ServiceMgr += TGCcablingSvc
