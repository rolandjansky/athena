# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb
from AthenaCommon.AlgSequence import AthSequencer
condseq = AthSequencer("AthCondSeq")

if not hasattr (condSeq,"LArHVCondAlg"):
   if conddb.isMC:
      from LArRecUtils.LArRecUtilsConf import LArHVCondAlg
      hvcond = LArHVCondAlg(doHV=False,doAffectedHV=False)
      condSeq+=hvcond
   else:
      from LArConditionsCommon import LArHVDB
