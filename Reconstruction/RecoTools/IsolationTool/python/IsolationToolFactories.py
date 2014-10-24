# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all CaloIsolationTool for egammaCaloTools with default configuration"
__author__ = "Nathan Readioff"

from egammaRec.Factories import FcnWrapper, ToolFactory, FullNameWrapper
from egammaRec import egammaKeys
#---------------------------------------

         
from egammaCaloTools.egammaCaloToolsFactories import CaloFillRectangularCluster

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
CaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name="xAOD__CaloIsolationTool_eg57",
                                CaloFillRectangularClusterTool = CaloFillRectangularCluster,
                                UseEMScale = True)
