# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration



from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KTSplittingScaleTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import AngularityTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import DipolarityTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import PlanarFlowTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtMassDropTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorGeneralizedRatiosTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import CenterOfMassShapesTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetPullTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetChargeTool

def declareDefaultTools():
  from JetRecConfig.JetRecStandardToolManager import jtm
  try:
    from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import ShowerDeconstructionTool
    jtm.haveShowerDeconstructionTool = True
  except ImportError:
    jtm.haveShowerDeconstructionTool = False

  #--------------------------------------------------------------
  # Substructure moment builders.
  #--------------------------------------------------------------

  # Nsubjettiness
  jtm += NSubjettinessTool(
    "nsubjettiness",
    Alpha = 1.0
  )

  # KtDR
  jtm += KtDeltaRTool(
    "ktdr",
    JetRadius = 0.4
  )

  # Kt-splitter
  jtm += KTSplittingScaleTool("ktsplitter")

  # Angularity.
  jtm += AngularityTool("angularity")

  # Dipolarity.
  jtm += DipolarityTool("dipolarity", SubJetRadius = 0.3)

  # Planar flow.
  jtm += PlanarFlowTool("planarflow")

  # Kt mass drop.
  jtm += KtMassDropTool("ktmassdrop")

  # Energy correlations.
  jtm += EnergyCorrelatorTool("encorr", Beta = 1.0)

  # COM shapes.
  jtm += CenterOfMassShapesTool("comshapes")

  # Jet pull
  jtm += JetPullTool(
    "pull",
    UseEtaInsteadOfY = False,
    IncludeTensorMoments = True
  )

  # Jet charge
  jtm += JetChargeTool("charge", K=1.0)

  # generalized ECFs
  jtm += EnergyCorrelatorGeneralizedTool("energycorrelatorgeneralized")

  # ... & their ratios
  jtm += EnergyCorrelatorGeneralizedRatiosTool("energycorrelatorgeneralizedratios")

  # Shower deconstruction.
  if jtm.haveShowerDeconstructionTool:
    jtm += ShowerDeconstructionTool("showerdec")
