# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate all EMShowerBuilder with default configuration"
__author__ = "Bruno Lenzi"

from AthenaCommon.Constants import INFO # OutputLevel
from egammaTools import egammaToolsConf
from egammaRec.Factories import ToolFactory, getPropertyValue
from egammaRec import egammaKeys
from egammaCaloTools.egammaCaloToolsFactories import egammaShowerShape, \
  egammaIso

import ROOT, cppyy
# Need to be sure base dict is loaded first.
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
EgammaParameters = ROOT.xAOD.Iso

from CaloIdentifier import SUBCALO        
from RecExConfig.RecFlags import rec

# TODO: not needed ?
# from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
# theCaloDepthTool=CaloDepthToolFactory(depth='showerdefault')
 
EMShowerBuilder = ToolFactory( egammaToolsConf.EMShowerBuilder,
        CellsName = egammaKeys.caloCellKey(),
        CaloNums  = [SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE],
        ShowerShapeTool      = egammaShowerShape,
        HadronicLeakageTool  = egammaIso,
        Print = False)

