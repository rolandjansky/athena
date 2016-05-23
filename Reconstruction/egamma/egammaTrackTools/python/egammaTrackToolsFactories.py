# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all egammaCaloTools with default configuration"
__author__ = "Bruno Lenzi"
from egammaRec.Factories import FcnWrapper, ToolFactory
import egammaTrackToolsConf
from egammaRec import egammaKeys
from egammaTools.InDetTools import egammaExtrapolator
import AthenaCommon.CfgMgr as CfgMgr
#--------------------------------------
EMDefaultCaloExtensionTool =  ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool)

EMParticleCaloExtensionTool =  ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool, 
                                            name="EMParticleCaloExtensionTool",
                                            Extrapolator = egammaExtrapolator,
                                            OutputContainerName="EGCaloExtension",
                                            ParticleType="muon",
                                            StartFromPerigee = True)

EMExtrapolationTools = ToolFactory( egammaTrackToolsConf.EMExtrapolationTools,
                                    DefaultCaloExtentionTool=EMDefaultCaloExtensionTool,
                                    PerigeeCaloExtentionTool = EMParticleCaloExtensionTool,
                                    Extrapolator=egammaExtrapolator,
                                    useCaching = True)

#---------------------------------------

# Import the factories that are not defined here
pass
