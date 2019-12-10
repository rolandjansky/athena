# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate all egammaCaloTools with default configuration"
__author__ = "Bruno Lenzi , Christos Anastopoulos"
from egammaRec.Factories import FcnWrapper, ToolFactory
from egammaTrackTools import egammaTrackToolsConf
from egammaRec import egammaKeys
from egammaTools.InDetTools import egammaCaloExtrapolator,egammaExtrapolator
import AthenaCommon.CfgMgr as CfgMgr

EMLastCaloExtensionTool =  ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool,
                                           name="EMLastCaloExtensionTool",
                                           Extrapolator = egammaCaloExtrapolator,
                                           ParticleType = "electron")

EMParticleCaloExtensionTool =  ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool, 
                                            name="EMParticleCaloExtensionTool",
                                            Extrapolator = egammaCaloExtrapolator,
                                            ParticleType="electron",
                                            StartFromPerigee = True)


EMExtrapolationTools = ToolFactory( egammaTrackToolsConf.EMExtrapolationTools,
                                    name ="EMExtrapolationTools",
                                    LastCaloExtensionTool=EMLastCaloExtensionTool,
                                    PerigeeCaloExtensionTool = EMParticleCaloExtensionTool,
                                    Extrapolator=egammaExtrapolator,
                                    useCaching=False,
                                    useLastCaching=False
                                    )


EMExtrapolationToolsCache = ToolFactory( egammaTrackToolsConf.EMExtrapolationTools,
                                         name ="EMExtrapolationToolsCache",
                                         LastCaloExtensionTool=EMLastCaloExtensionTool,
                                         PerigeeCaloExtensionTool = EMParticleCaloExtensionTool,
                                         Extrapolator=egammaExtrapolator,
                                         PerigeeCache='GSFPerigeeCaloExtension',
                                         LastCache='GSFLastCaloExtension',
                                         useCaching=True,
                                         useLastCaching=True
                                         )

EMExtrapolationToolsCommonCache = ToolFactory( egammaTrackToolsConf.EMExtrapolationTools,
                                               name ="EMExtrapolationToolsCommonCache",
                                               LastCaloExtensionTool=EMLastCaloExtensionTool,
                                               PerigeeCaloExtensionTool = EMParticleCaloExtensionTool,
                                               Extrapolator=egammaExtrapolator,
                                               LastCache='ParticleCaloExtension',
                                               useCaching=False,
                                               useLastCaching=True
                                             )

