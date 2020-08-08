# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from CaloClusterCorrection import CaloClusterCorrectionConf as Cccc
__doc__ = """ToolFactories to instantiate all
egammaCaloTools with default configuration"""
__author__ = "Bruno Lenzi"

from egammaRec.Factories import ToolFactory
from egammaCaloTools import egammaCaloToolsConf
from egammaRec import egammaKeys

# ---------------------------------------

egammaShowerShape = ToolFactory(
    egammaCaloToolsConf.egammaShowerShape,
    # Samplings to execute
    ExecAllVariables=True,
    ExecPreSampler=True,
    ExecEMFirst=True,
    ExecEMSecond=True,
    ExecEMThird=True,
    ExecEMCombined=True)


CaloFillRectangularCluster = ToolFactory(
    Cccc.CaloFillRectangularCluster,
    name="egamma_CaloFillRectangularCluster",
    eta_size=5,
    phi_size=7,
    cells_name=egammaKeys.caloCellKey())


egammaIso = ToolFactory(
    egammaCaloToolsConf.egammaIso)

egammaCheckEnergyDepositTool = ToolFactory(
    egammaCaloToolsConf.egammaCheckEnergyDepositTool)


# ---------------------------------------
