# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, METConfig

cfg_calo = METConfig('Calo',
                     [BuildConfig('CaloReg')],
                     doCells=True
                     )

metFlags.METConfigs()[cfg_calo.suffix] = cfg_calo
metFlags.METOutputList().append(cfg_calo.suffix)

