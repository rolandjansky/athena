# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoCfg import BuildConfig, RefConfig, METConfig,getMETRecoAlg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def METTrack_Cfg(configFlags):
    sequencename = "METReconstruction_Track"

    components = ComponentAccumulator()
    from AthenaConfiguration.ComponentFactory import CompFactory
    AthSequencer=CompFactory.AthSequencer
    components.addSequence( AthSequencer(sequencename) )
    cfg_trk = METConfig('Track',configFlags,[BuildConfig('SoftTrk','Track')],
                    [RefConfig('TrackFilter','PVTrack')],
                    doTracks=configFlags.MET.UseTracks)

    cfg_trk.refiners['TrackFilter'].DoLepRecovery=True
    cfg_trk.refiners['TrackFilter'].DoVxSep=configFlags.MET.UseTracks
    cfg_trk.refiners['TrackFilter'].DoEoverPSel=True
    recoAlg=getMETRecoAlg(algName='METRecoAlg_Track',configs={"Track":cfg_trk})
    components.addEventAlgo(recoAlg, sequencename)
    return components

