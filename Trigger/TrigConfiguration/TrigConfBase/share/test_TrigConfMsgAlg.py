# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from TrigConfBase.TrigConfBaseTestConf import TrigConfMsgAlg
job += TrigConfMsgAlg()

theApp.EvtMax = 2
