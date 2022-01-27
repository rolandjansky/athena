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

    AthReadAlg_ExtraInputs = []

    # Configure Super Cell Alignment condition algorithm
    # Calo super cell building works only if both LAr and Tile are present
    # And we need to make sure this is a Run3 geometry layout

    from RecExConfig.AutoConfiguration import IsInInputFile
    caloCellsInInput = IsInInputFile('CaloCellContainer')
    sCellsInInput = False
    caloCellKeys = []
    if caloCellsInInput:
      from PyUtils.MetaReaderPeeker import convert_itemList
      caloCellKeys = convert_itemList(layout='dict')
      for key in caloCellKeys['CaloCellContainer']:
        if key != 'AllCalo':
          sCellsInInput = True

    AthReadAlg_ExtraInputs.append(('CaloDetDescrManager', 'ConditionStore+CaloDetDescrManager'))
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    if commonGeoFlags.Run() == "RUN3" and DetFlags.detdescr.Tile_on() or sCellsInInput:
      from CaloAlignmentAlgs.CaloAlignmentAlgsConf import CaloSuperCellAlignCondAlg
      condSeq += CaloSuperCellAlignCondAlg("CaloSuperCellAlignCondAlg")
      AthReadAlg_ExtraInputs.append(('CaloSuperCellDetDescrManager', 'ConditionStore+CaloSuperCellDetDescrManager'))

    if caloCellsInInput:
      from AthenaConfiguration.ComponentFactory import CompFactory
      AthReadAlg=CompFactory.AthReadAlg
      for key in caloCellKeys['CaloCellContainer']:
        AthReadAlg_CaloCellCont = AthReadAlg (f'AthReadAlg_{key}',
                                              Key = f'CaloCellContainer/{key}',
                                              Aliases = [],
                                              ExtraInputs = AthReadAlg_ExtraInputs)
        condSeq += AthReadAlg_CaloCellCont
