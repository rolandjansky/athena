# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon import CfgMgr
 
def getInDetTRTStrawStatusSummaryTool(name='InDetTRTStrawStatusSummaryTool', **kwargs):
     return CfgMgr.TRT_StrawStatusSummaryTool(name, **kwargs)
