# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TrkEventCnvSuperToolCfg(flags):
    acc = ComponentAccumulator()
    Trk_EventCnvSuperTool = CompFactory.Trk.EventCnvSuperTool
    EventCnvSuperTool = Trk_EventCnvSuperTool(
        'EventCnvSuperTool',
        DoID=flags.Reco.EnableTracking,
        DoMuons=flags.Detector.EnableMuon,
    )
    if (flags.Common.isOverlay or flags.Output.doWriteRDO) and flags.Overlay.TrackOverlay:
        EventCnvSuperTool.DoTrackOverlay=True
    acc.addPublicTool(EventCnvSuperTool)
    return acc
