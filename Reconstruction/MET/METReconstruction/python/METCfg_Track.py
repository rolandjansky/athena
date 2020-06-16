# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoCfg import BuildConfig, RefConfig, METConfig,getMETRecoAlg,getMETRecoTool
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def METTrack_Cfg(configFlags):
    sequencename = "METReconstruction_Track"

    components = ComponentAccumulator()
    from AthenaCommon.AlgSequence import AthSequencer
    components.addSequence( AthSequencer(sequencename) )

    cfg_trk = METConfig('Track',[BuildConfig('SoftTrk','Track')],
                    [RefConfig('TrackFilter','PVTrack')],
                    doTracks=configFlags.MET.UseTracks)

    cfg_trk.refiners['TrackFilter'].DoLepRecovery=True
    cfg_trk.refiners['TrackFilter'].DoVxSep=configFlags.MET.UseTracks
    cfg_trk.refiners['TrackFilter'].DoEoverPSel=True
    recotool = getMETRecoTool(cfg_trk)
    #recotool = CfgMgr.met__METRecoTool('MET_RecoTool_empty')
    recoAlg=getMETRecoAlg(algName='METRecoAlg_Track',tools=[recotool])
    components.addEventAlgo(recoAlg, sequencename)
    return components

