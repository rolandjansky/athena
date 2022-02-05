# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
from LArCafJobs.LArCafJobsConfig import DefaultShapeDumperTool
from LArSamplesMon.LArSamplesMonConf import LArShapeCompleteMaker

class DefaultLArShapeCompleteMaker(LArShapeCompleteMaker) :

  def __init__(self, name = 'LArShapeCompleteMaker', 
               shapeErrorFileName = '', outputFileName = '', outputTemplateFileName = '',
               outputKey = '', nSamples = 5, nPhases = 17, iPhase = 8) :
    super(LArShapeCompleteMaker, self).__init__(name)

    from LArCabling.LArCablingAccess import LArOnOffIdMapping
    LArOnOffIdMapping()

    from AthenaCommon.AppMgr import ToolSvc
    theDumperTool = DefaultShapeDumperTool(doShape=False)
    ToolSvc += theDumperTool

    self.ShapeErrorFileName = shapeErrorFileName
    self.OutputFileName = outputFileName 
    self.OutputTemplateFileName = outputTemplateFileName 
    self.OutputKey = outputKey
    self.NSamples = nSamples
    self.NPhases = nPhases
    self.IPhase = iPhase
