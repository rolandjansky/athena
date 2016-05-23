# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all egammaCaloTools with default configuration"
__author__ = "Bruno Lenzi"

from egammaRec.Factories import FcnWrapper, ToolFactory
import egammaCaloToolsConf
from egammaRec import egammaKeys

#---------------------------------------

egammaShowerShape = ToolFactory(egammaCaloToolsConf.egammaShowerShape,
                                # Samplings to execute
                                ExecAllVariables   = True,
                                ExecPreSampler     = True,
                                ExecEMFirst        = True,
                                ExecEMSecond       = True,
                                ExecEMThird        = True,
                                ExecEMCombined     = True,
                                ExecOtherVariables = True )

            
from CaloClusterCorrection import CaloClusterCorrectionConf as Cccc
CaloFillRectangularCluster = ToolFactory( Cccc.CaloFillRectangularCluster, 
                                          name = "egamma_CaloFillRectangularCluster",
                                          eta_size = 5,
                                          phi_size = 7,
                                          cells_name = egammaKeys.caloCellKey() )




egammaIso = ToolFactory(egammaCaloToolsConf.egammaIso)

#---------------------------------------

