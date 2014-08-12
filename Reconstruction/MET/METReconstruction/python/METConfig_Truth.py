# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, METConfig

cfg_truth = METConfig('Truth',
                      [BuildConfig('NonInt'),
                       BuildConfig('Int'),
                       BuildConfig('IntOut'),
                       BuildConfig('IntMuons')],
                      doRegions=True
                      )

metFlags.METConfigs()[cfg_truth.suffix] = cfg_truth
metFlags.METOutputList().append(cfg_truth.suffix)
metFlags.METOutputList().append(cfg_truth.suffix+"Regions")

