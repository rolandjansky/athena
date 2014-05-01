# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from xAODJetCnv.xAODJetCnvConf import JetGlobalEventSetup
from AthenaCommon.AlgSequence import AlgSequence
job =AlgSequence()
job += JetGlobalEventSetup()
