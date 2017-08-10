# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getInDetTRTStrawStatusSummarySvc(name='InDetTRTStrawStatusSummarySvc', **kwargs):
    return CfgMgr.TRT_StrawStatusSummarySvc(name, **kwargs)
