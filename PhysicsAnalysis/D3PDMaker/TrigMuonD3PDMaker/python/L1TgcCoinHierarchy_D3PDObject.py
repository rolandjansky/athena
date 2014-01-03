# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import TrigMuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from AthenaCommon.AppMgr import ToolSvc


def makeL1TgcCoinHierarchy_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
  return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix, ObjectName = object_name)

L1TgcCoinHierarchy_D3PDObject = D3PDObject(makeL1TgcCoinHierarchy_D3PDObject,
                                           default_object_name = 'L1TgcCoinHierarchy_D3PDObject',
                                           default_name = 'L1TgcCoinHierarchyFiller')

L1TgcCoinHierarchy_D3PDObject.defineBlock(lod = 0,
                                          name = 'L1TgcCoinHierarchy',
                                          func = TrigMuonD3PDMaker.L1TgcCoinHierarchyFillerTool,
                                          prefix = 'TGC_hierarchy_')
# eof
