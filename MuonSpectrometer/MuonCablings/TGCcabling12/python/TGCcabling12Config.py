# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TGCcabling12.TGCcabling12Conf import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr

class TGCcabling12Config (TGCcabling12Svc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(TGCcabling12Config ,self).__init__(name)

TGCcablingSvc = TGCcabling12Config()
ServiceMgr += TGCcablingSvc

