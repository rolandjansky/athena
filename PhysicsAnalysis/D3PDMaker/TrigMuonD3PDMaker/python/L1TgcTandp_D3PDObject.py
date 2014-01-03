# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import TrigMuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from AthenaCommon.AppMgr import ToolSvc



def makeL1TgcTandp_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
  return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix, ObjectName = object_name)

L1TgcStacoTandp_D3PDObject = D3PDObject(makeL1TgcTandp_D3PDObject,
                                        default_object_name = 'L1TgcStacoTandp_D3PDObject',
                                        default_name = 'L1TgcStacoTandpFiller')

L1TgcStacoTandp_D3PDObject.defineBlock(lod = 0,
                                        name = 'L1TgcStacoTandp',
                                        func = TrigMuonD3PDMaker.L1TgcTandpFillerTool,
                                        prefix = 'tandp_staco_',
                                        MuonContainer='StacoMuonCollection')


L1TgcMuidTandp_D3PDObject = D3PDObject(makeL1TgcTandp_D3PDObject,
                                       default_object_name = 'L1TgcMuidTandp_D3PDObject',
                                       default_name = 'L1TgcMuidTandpFiller')

L1TgcMuidTandp_D3PDObject.defineBlock(lod = 0,
                                      name = 'L1TgcMuidTandp',
                                      func = TrigMuonD3PDMaker.L1TgcTandpFillerTool,
                                      prefix = 'tandp_muid_',
                                      MuonContainer='MuidMuonCollection')
# eof
