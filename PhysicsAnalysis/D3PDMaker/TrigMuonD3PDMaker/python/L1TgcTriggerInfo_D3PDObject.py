# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import TrigMuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from AthenaCommon.AppMgr import ToolSvc


def makeL1TgcTriggerInfo_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
  return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix, ObjectName = object_name)

L1TgcTriggerInfo_D3PDObject = D3PDObject(makeL1TgcTriggerInfo_D3PDObject,
                                         default_object_name = 'L1TgcTriggerInfo_D3PDObject',
                                         default_name = 'L1TgcTriggerInfoFiller')

pattern = "(EF_mu[0-9]+i?((_loose)|(_medium)|(_tight))?)"
pattern += "| (EF_mu[0-9]+_MG((_loose)|(_medium)|(_tight))?)"
pattern += "| (EF_mu[0-9]+_Trk_Jpsi((_loose)|(_medium)|(_tight))?)"
pattern += "| (EF_mu[0-9]+_MSonly((_loose)|(_medium)|(_tight)|(_tighter)|(_cosmic)|(_empty))?)"
pattern += "| (EF_mu[0-9]+_IDTrkNoCut)"
pattern += "| (EF_L1MU[0-9]+_NoAlg)"

L1TgcTriggerInfo_D3PDObject.defineBlock(lod = 0,
                                        name = 'L1TgcTriggerInfo',
                                        func = TrigMuonD3PDMaker.L1TgcTriggerInfoFillerTool,
                                        prefix = 'trigger_info_',
                                        TriggerPattern = pattern)

from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
defineTriggerBits(L1TgcTriggerInfo_D3PDObject, 1, D3PDMakerFlags.MuonEFTrigPattern())
# eof
