# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Job options needed to enable LAr alignment

from AthenaCommon.DetFlags import DetFlags
import os
activateCondAlgs = (not DetFlags.simulate.any_on() or DetFlags.overlay.any_on()) and "AthSimulation_DIR" not in os.environ

from IOVDbSvc.CondDB import conddb

if conddb.isMC:
  #Monte Carlo case:
  if activateCondAlgs:
    conddb.addFolder("LAR_OFL","/LAR/Align",className="DetCondKeyTrans")
    conddb.addFolder("LAR_OFL","/LAR/LArCellPositionShift",className="CaloRec::CaloCellPositionShift")
  else:
    conddb.addFolder("LAR_OFL","/LAR/Align")
    conddb.addFolder("LAR_OFL","/LAR/LArCellPositionShift")
else:
  #Regular offline data processing
  conddb.addFolder("LAR_ONL","/LAR/Align",className="DetCondKeyTrans")
  conddb.addFolder("LAR_ONL","/LAR/LArCellPositionShift",className="CaloRec::CaloCellPositionShift")

if activateCondAlgs:
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq,"LArAlignCondAlg"):
    from LArAlignmentAlgs.LArAlignmentAlgsConf import LArAlignCondAlg
    condSeq += LArAlignCondAlg("LArAlignCondAlg")
    from CaloAlignmentAlgs.CaloAlignmentAlgsConf import CaloAlignCondAlg
    condSeq += CaloAlignCondAlg("CaloAlignCondAlg")
