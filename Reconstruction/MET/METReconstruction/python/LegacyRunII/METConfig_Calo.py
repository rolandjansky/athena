# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig

############################################################################
# EMTopo

cfg_emt = METConfig('EMTopo',[BuildConfig('SoftClus','EMTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )

metFlags.METConfigs()[cfg_emt.suffix] = cfg_emt
metFlags.METOutputList().append(cfg_emt.suffix)
metFlags.METOutputList().append(cfg_emt.suffix+"Regions")

############################################################################
# LocHadTopo

cfg_lht = METConfig('LocHadTopo',[BuildConfig('SoftClus','LocHadTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )

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
