# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
import TrigMuonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from AthenaCommon.AppMgr import ToolSvc


def makeL1TgcTrkExtrapolation_D3PDObject(name, prefix, object_name, getter = None, sgkey = '', method = ''):
  return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix, ObjectName = object_name)


L1TgcStacoExtrapolation_D3PDObject = D3PDObject(makeL1TgcTrkExtrapolation_D3PDObject,
                                                default_object_name = 'L1TgcStacoExtrapolation_D3PDObject',
                                                default_name = 'L1TgcStacoExtrapolationFiller')

L1TgcStacoExtrapolation_D3PDObject.defineBlock(lod = 0,
                                               name = 'L1TgcStacoExtrapolation',
                                               func = TrigMuonD3PDMaker.L1TgcTrkExtrapolationFillerTool,
                                               prefix = 'ext_staco_ubias_',
                                               MuonContainer='StacoMuonCollection',
                                               ExtrapolationMethod='UNBIASED')



L1TgcMuidExtrapolation_D3PDObject = D3PDObject(makeL1TgcTrkExtrapolation_D3PDObject,
                                               default_object_name = 'L1TgcMuidExtrapolation_D3PDObject',
                                               default_name = 'L1TgcMuidExtrapolationFiller')

L1TgcMuidExtrapolation_D3PDObject.defineBlock(lod = 0,
                                              name = 'L1TgcMuidExtrapolation',
                                              func = TrigMuonD3PDMaker.L1TgcTrkExtrapolationFillerTool,
                                              prefix = 'ext_muid_ubias_',
                                              MuonContainer='MuidMuonCollection',
                                              ExtrapolationMethod='UNBIASED')



L1TgcCaloExtrapolation_D3PDObject = D3PDObject(makeL1TgcTrkExtrapolation_D3PDObject,
                                               default_object_name = 'L1TgcCaloExtrapolation_D3PDObject',
                                               default_name = 'L1TgcCaloExtrapolationFiller')

L1TgcCaloExtrapolation_D3PDObject.defineBlock(lod = 0,
                                              name = 'L1TgcCaloExtrapolation',
                                              func = TrigMuonD3PDMaker.L1TgcTrkExtrapolationFillerTool,
                                              prefix = 'ext_calo_ubias_',
                                              MuonContainer='CaloMuonCollection',
                                              ExtrapolationMethod='UNBIASED')



L1TgcStacoBiasedExtrapolation_D3PDObject = D3PDObject(makeL1TgcTrkExtrapolation_D3PDObject,
                                                      default_object_name = 'L1TgcStacoBiasedExtrapolation_D3PDObject',
                                                      default_name = 'L1TgcStacoBiasedExtrapolationFiller')

L1TgcStacoBiasedExtrapolation_D3PDObject.defineBlock(lod = 0,
                                                     name = 'L1TgcStacoBiasedExtrapolation',
                                                     func = TrigMuonD3PDMaker.L1TgcTrkExtrapolationFillerTool,
                                                     prefix = 'ext_staco_bias_',
                                                     MuonContainer='StacoMuonCollection',
                                                     ExtrapolationMethod='BIASED')



L1TgcMuidBiasedExtrapolation_D3PDObject = D3PDObject(makeL1TgcTrkExtrapolation_D3PDObject,
                                                     default_object_name = 'L1TgcMuidBiasedExtrapolation_D3PDObject',
                                                     default_name = 'L1TgcMuidBiasedExtrapolationFiller')

L1TgcMuidBiasedExtrapolation_D3PDObject.defineBlock(lod = 0,
                                                    name = 'L1TgcMuidBiasedExtrapolation',
                                                    func = TrigMuonD3PDMaker.L1TgcTrkExtrapolationFillerTool,
                                                    prefix = 'ext_muid_bias_',
                                                    MuonContainer='MuidMuonCollection',
                                                    ExtrapolationMethod='BIASED')

# T.Kanno from release 18, the muon container called 'Muons' will be the standard.
L1TgcMuonsExtrapolation_D3PDObject = D3PDObject(makeL1TgcTrkExtrapolation_D3PDObject,
                                                default_object_name = 'L1TgcMuonsExtrapolation_D3PDObject',
                                                default_name = 'L1TgcMuonsExtrapolationFiller')

L1TgcMuonsExtrapolation_D3PDObject.defineBlock(lod = 0,
                                               name = 'L1TgcMuonsExtrapolation',
                                               func = TrigMuonD3PDMaker.L1TgcTrkExtrapolationFillerTool,
                                               prefix = 'ext_muons_ubias_',
                                               MuonContainer='Muons',
                                               ExtrapolationMethod='UNBIASED')
#T.K


# eof
