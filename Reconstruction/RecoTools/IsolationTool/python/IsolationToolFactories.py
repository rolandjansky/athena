# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all CaloIsolationTool for egammaCaloTools with default configuration"
__author__ = "Nathan Readioff"

from AthenaCommon.Constants import INFO

from egammaRec.Factories import ToolFactory, getPropertyValue
from CaloIdentifier import SUBCALO 
#---------------------------------------
# egamma specifics          
from egammaCaloTools.egammaCaloToolsFactories import CaloFillRectangularCluster

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInConeTool = ToolFactory(xAOD__CaloClustersInConeTool,
                                     CaloClusterLocation = "CaloCalTopoCluster")

from AthenaCommon.GlobalFlags import globalflags
isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ICT
IsoCorrectionTool = ToolFactory(ICT,
                                name = "NewLeakageCorrTool",
                                IsMC = isMC)

# configuration for ED computation
def configureEDCorrection(tool):
  """Configure tools and algorithm for energy density correction 
     (only if doEnergyDensityCorrection = True)"""
  # Return if doEnergyDensityCorrection is false
  if not getPropertyValue(tool, 'doEnergyDensityCorrection'):
    return
  # Set OutputLevel to INFO or higher if tool has it too
  OutputLevel = min(getPropertyValue(tool, 'OutputLevel'), INFO)
  try:
    from AthenaCommon.AppMgr import ToolSvc
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
    from JetRec.JetRecStandard import jtm
    tc=configEventDensityTool("EDtpIsoCentralTool", jtm.emget,
                              radius          = 0.5,
                              AbsRapidityMin  = 0.0,
                              AbsRapidityMax  = 1.5,
                              OutputContainer = "DensitytpIsoCentral",
                              OutputLevel     = OutputLevel
                              )
    ToolSvc += tc
    tf=configEventDensityTool("EDtpIsoForwardTool", jtm.emget,
                              radius          = 0.5,
                              AbsRapidityMin  = 1.5,
                              AbsRapidityMax  = 3.0,
                              OutputContainer = "DensitytpIsoForward",
                              OutputLevel     = OutputLevel
                              )
    ToolSvc += tf
  except Exception:
    print '\nERROR: could not get handle to EDEgammaXTool'
    raise
 
  # add to topsequence 
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  if not hasattr(topSequence,'EDtpIsoCentralAlg'):
    topSequence += EventDensityAlg("EDtpIsoCentralAlg", EventDensityTool = tc)
  if not hasattr(topSequence,'EDtpIsoForwardAlg'):
    topSequence += EventDensityAlg("EDtpIsoForwardAlg", EventDensityTool = tf)


from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
CaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool, name="CaloIsolationTool",
                                postInit = [configureEDCorrection],
                                CaloFillRectangularClusterTool  = CaloFillRectangularCluster,
                                IsoLeakCorrectionTool           = IsoCorrectionTool,
                                ClustersInConeTool              = CaloClustersInConeTool,
                                EMCaloNums  = [SUBCALO.LAREM],
                                HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE],
                                UseEMScale = True
                                ExcludeTG3 = True)
