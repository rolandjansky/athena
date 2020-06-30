# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate all EMShowerBuilder
with default configuration"""
__author__ = "Bruno Lenzi"


from egammaTools import egammaToolsConf
from egammaRec.Factories import ToolFactory
from egammaRec import egammaKeys
from egammaCaloTools.egammaCaloToolsFactories import (
  egammaShowerShape, egammaIso)
from CaloIdentifier import SUBCALO


EMShowerBuilder = ToolFactory(
    egammaToolsConf.EMShowerBuilder,
    CellsName=egammaKeys.caloCellKey(),
    CaloNums=[SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE],
    ShowerShapeTool=egammaShowerShape,
    HadronicLeakageTool=egammaIso,
    Print=False)
