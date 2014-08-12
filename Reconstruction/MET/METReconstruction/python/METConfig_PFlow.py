# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoConfig import BuildConfig, RefConfig, METConfig

############################################################################
# Simple PFlow

cfg_pflow = METConfig('PFlow',[BuildConfig('SoftPFlow','PFlow')])

metFlags.METConfigs()[cfg_pflow.suffix] = cfg_pflow
metFlags.METOutputList().append(cfg_pflow.suffix)
metFlags.METOutputList().append(cfg_pflow.suffix+"Regions")
