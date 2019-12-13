# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate egammaSelectedTrackCopy with default configuration"
__author__ = "Christos"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import ToolFactory, AlgFactory
from egammaRec.egammaRecFlags import jobproperties # to set jobproperties.egammaRecFlags
from egammaRec import egammaKeys
from InDetRecExample.InDetKeys import InDetKeys

from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools, EMExtrapolationToolsCommonCache
from egammaCaloTools import egammaCaloToolsConf

egammaCaloClusterGSFSelector = ToolFactory( egammaCaloToolsConf.egammaCaloClusterSelector,
                                            name = 'caloClusterGSFSelector',
                                            EMEtRanges = [2500.],
                                            EMFCuts = [0.7]
                                          )  

egammaSelectedTrackCopy = AlgFactory( egammaAlgsConf.egammaSelectedTrackCopy,
                                      name = 'egammaSelectedTrackCopy' ,
                                      ExtrapolationTool = EMExtrapolationTools,
                                      ExtrapolationToolCommonCache = EMExtrapolationToolsCommonCache,
                                      ClusterContainerName=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
                                      TrackParticleContainerName=InDetKeys.xAODTrackParticleContainer(),
                                      minNoSiHits=4,
                                      broadDeltaEta=0.1,   # this is multiplied by 2 for the Candidate Match , so +- 0.2 in eta
                                      broadDeltaPhi=0.15,   # this is multiplied by 2 for the Candidate Match , so +- 0.3 in phi
                                      narrowDeltaEta=0.05, 
                                      #These have to be relaxed enough for the conversions
                                      narrowDeltaPhi=0.05,   
                                      narrowDeltaPhiBrem=0.20, #Dominated by the needs of assymetric conversions
                                      narrowDeltaPhiRescale=0.05,  
                                      narrowDeltaPhiRescaleBrem=0.1,
                                      egammaCaloClusterSelector = egammaCaloClusterGSFSelector
                                      )
