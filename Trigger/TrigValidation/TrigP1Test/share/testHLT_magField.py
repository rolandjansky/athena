# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Testing job options for magnetic field changes. Can be used e.g. in the
# "magFieldToggle" test instead of the full menu. Only runs one algorithm
# that prints the field status.

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AlgSequence import AthSequencer, AlgSequence

globalflags.DataSource='data'
globalflags.ConditionsTag='CONDBR2-HLTP-2018-01'

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())

import MagFieldServices.SetupField  # noqa: F401

from MagFieldUtils.MagFieldUtilsConf import MagField__CondReader
topSequence = AlgSequence()
topSequence += MagField__CondReader("MagFieldCondReader")

condSeq = AthSequencer('AthCondSeq')
condSeq.AtlasFieldMapCondAlg.LoadMapOnStart = True
condSeq.AtlasFieldMapCondAlg.UseMapsFromCOOL = False
condSeq.AtlasFieldCacheCondAlg.LockMapCurrents = True
condSeq.AtlasFieldCacheCondAlg.UseDCS = False

condSeq.AtlasFieldMapCondAlg.OutputLevel = DEBUG
condSeq.AtlasFieldCacheCondAlg.OutputLevel = DEBUG

theApp.EvtMax = 100   # noqa: F821 (for athena running)

