# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig,clusterSigStates

from RecExConfig.RecFlags import rec
doInDet = rec.doInDet()

cfg_trk = METConfig('Track',[BuildConfig('SoftTrk','Track')],
                    [RefConfig('TrackFilter','PVTrack')],
                    doTracks=doInDet)

cfg_trk.refiners['TrackFilter'].DoLepRecovery=True
cfg_trk.refiners['TrackFilter'].DoVxSep=True
# Old input names
#cfg_trk.refiners['TrackFilter'].InputClusterKey="CaloCalTopoCluster"
#cfg_trk.refiners['TrackFilter'].InputElectronKey="ElectronCollection"
metFlags.METConfigs()[cfg_trk.suffix] = cfg_trk
metFlags.METOutputList().append(cfg_trk.suffix)
