# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# specifies Calo cell making
# so far only handle the RawChannel->CaloCell step
# not all possibility of CaloCellMaker_jobOptions.py integrated yet

from RecExConfig.Configured import Configured

class SubtractedCellGetter ( Configured )  :
    _outputType = "CaloCellContainer"
    _output = { _outputType : "SubtractedCells" }

    def configure(self):
        from CaloRec.CaloRecConf import CaloCellMaker
        cellMaker = CaloCellMaker("HICaloCellCopier",CaloCellsOutputName="SubtractedCells",OwnPolicy = 0)
        from CaloRec.CaloRecConf import CaloCellFastCopyTool
        cellCopyTool = CaloCellFastCopyTool("HICellCopyTool")
        cellCopyTool.InputName ="AllCalo"
        cellCopyTool.IncludeSamplings = [ "PreSamplerB", "EMB1", "EMB2", "EMB3",
                                          "PreSamplerE", "EME1", "EME2", "EME3",
                                          "HEC0", "HEC1", "HEC2", "HEC3",
                                          "TileBar0", "TileBar1", "TileBar2",
                                          "TileGap1", "TileGap2", "TileGap3",
                                          "TileExt0", "TileExt1", "TileExt2",
                                          "FCal1", "FCal2", "FCal3" ]

        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += cellCopyTool

        from HIJetRec.HIJetRecConf import HISubtractedCellMakerTool
        cellSubtrTool=HISubtractedCellMakerTool()
        from HIJetRec.HIJetRecFlags import HIJetFlags
        cellSubtrTool.EventShapeKey=HIJetFlags.IteratedEventShapeKey()
        from HIJetRec.HIJetRecTools import jtm
        cellSubtrTool.Modulator=jtm.modulator
        from HIEventUtils.HIEventUtilsConf import HIEventShapeMapTool
        theMapTool=HIEventShapeMapTool()
        cellSubtrTool.EventShapeMapTool=theMapTool
        ToolSvc += cellSubtrTool

        from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
        cellFinalizerTool = CaloCellContainerFinalizerTool("HICaloCellFinalizerTool")
        ToolSvc += cellFinalizerTool

        cellMaker.CaloCellMakerToolNames = [cellCopyTool,cellSubtrTool,cellFinalizerTool]
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence+=cellMaker
        
        return True

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType
