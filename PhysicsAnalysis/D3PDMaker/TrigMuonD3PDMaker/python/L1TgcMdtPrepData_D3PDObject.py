# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import TrigMuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from AthenaCommon.AppMgr import ToolSvc


def makeL1TgcMdtPrepData_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
  return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix, ObjectName = object_name)

L1TgcMdtPrepData_D3PDObject = D3PDObject(makeL1TgcMdtPrepData_D3PDObject,
                                         default_object_name = 'L1TgcMdtPrepData_D3PDObject',
                                         default_name = 'L1TgcMdtPrepDataFiller')

L1TgcMdtPrepData_D3PDObject.defineBlock(lod = 0,
                                        name = 'L1TgcMdtPrepData',
                                        func = TrigMuonD3PDMaker.L1TgcMdtPrepDataFillerTool,
                                        prefix = 'MDT_')
# eof
