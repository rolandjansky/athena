# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoCfg import BuildConfig, RefConfig, METConfig,clusterSigStates,getMETRecoAlg
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
    print "MET track helper object defined!!!"
    print cfg_trk
    recoAlg=getMETRecoAlg(sequencename,{'Track':cfg_trk})
    print "ALG defined"
    print recoAlg
    components.addEventAlgo(recoAlg, sequencename)
    return components

