# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#jobOptions fragment to fill LAr error bit during duration of noise burts

from IOVDbSvc.CondDB import conddb
conddb.addFolder("LAR_OFL","/LAR/BadChannelsOfl/EventVeto",className="AthenaAttributeList")
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq.CondInputLoader.Load+=[("AthenaAttributeList","/LAR/BadChannelsOfl/EventVeto")]

#conddb.addOverride("/LAR/BadChannelsOfl/EventVeto","LARBadChannelsOflEventVeto-empty")

from LArCellRec.LArCellRecConf import LArTimeVetoAlg
theLArTimeVetoAlg = LArTimeVetoAlg("LArTimeVetoAlg")
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theLArTimeVetoAlg
