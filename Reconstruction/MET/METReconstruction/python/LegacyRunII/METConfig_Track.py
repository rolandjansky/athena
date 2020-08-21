# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig

cfg_trk = METConfig('Track',[BuildConfig('SoftTrk','Track')],
                    [RefConfig('TrackFilter','PVTrack')],
                    doTracks=metFlags.UseTracks)

cfg_trk.refiners['TrackFilter'].DoLepRecovery=True
cfg_trk.refiners['TrackFilter'].DoVxSep=metFlags.UseTracks()
cfg_trk.refiners['TrackFilter'].DoEoverPSel=True

metFlags.METConfigs()[cfg_trk.suffix] = cfg_trk
metFlags.METOutputList().append(cfg_trk.suffix)
