# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig,clusterSigStates
#from METReconstruction.METRecoConfig_Associator import BuildConfig, RefConfig, METConfig,clusterSigStates

############################################################################
# EMTopo

cfg_emt = METConfig('EMTopo',[BuildConfig('SoftClus','EMTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )
cfg_emt.builders['SoftClus'].SignalState = clusterSigStates['EMScale']

metFlags.METConfigs()[cfg_emt.suffix] = cfg_emt
metFlags.METOutputList().append(cfg_emt.suffix)
metFlags.METOutputList().append(cfg_emt.suffix+"Regions")

############################################################################
# LocHadTopo

cfg_lht = METConfig('LocHadTopo',[BuildConfig('SoftClus','LocHadTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )
cfg_lht.builders['SoftClus'].SignalState = clusterSigStates['LocHad']

metFlags.METConfigs()[cfg_lht.suffix] = cfg_lht
metFlags.METOutputList().append(cfg_lht.suffix)
metFlags.METOutputList().append(cfg_lht.suffix+"Regions")


############################################################################
# Calo regions

cfg_calo = METConfig('Calo',
                     [BuildConfig('CaloReg')],
                     doCells=True
                     )

metFlags.METConfigs()[cfg_calo.suffix] = cfg_calo
metFlags.METOutputList().append(cfg_calo.suffix)
