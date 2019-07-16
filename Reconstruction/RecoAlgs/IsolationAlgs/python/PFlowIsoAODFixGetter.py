# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Logging import logging

from egammaRec.Factories import ToolFactory, AlgFactory

#---------------------------------------

# tool to collect pflow objects in cone
from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool as RetrievePFOTool
pfoTool = RetrievePFOTool();
ToolSvc += pfoTool

from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__PFlowObjectsInConeTool
PFlowObjectsInConeTool = ToolFactory(xAOD__PFlowObjectsInConeTool,
                                     name = "PFlowObjectsInConeTool",
                                     RetrievePFOTool = pfoTool)

from IsolationAlgs.IsolationAlgsConf import IsolationPflowCorrection
isoAODFixPflowCorrection = AlgFactory(IsolationPflowCorrection,
                                         name                  = "IsolationPflowCorrection",
                                         PFlowObjectsInConeTool          = PFlowObjectsInConeTool,
                                         ElectronCollectionContainerName    = "Electrons",
                                         PhotonCollectionContainerName    = "Photons")

from RecExConfig.Configured import Configured
class pflowIsoAODFixGetter ( Configured ) :
    
  def __init__(self):
    super( pflowIsoAODFixGetter, self ).__init__()

    mlog = logging.getLogger ('pflowIsoAODFixGetter.py::configure:')
    mlog.info('entering')
    
    # configure iso here:
    self._isoPflowCorrectionHandle = isoAODFixPflowCorrection()
         
  def pflowIsoBuilderHandle(self):
    return self._BuilderHandle

