# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Avoid double inclusion:
#include.block( "TrigBphysHypo/TrigBphysVertexingConfig.py" )

# Set up the TrigBphysHelperUtils to use special configuration of VertexFitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
from TrigBphysHypo.TrigBphysHypoConf       import TrigBphysHelperUtilsTool

from AthenaCommon.AppMgr import ToolSvc
#from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
#TrigBphysExtrapolator = AtlasExtrapolator()
#ToolSvc += TrigBphysExtrapolator
#print      TrigBphysExtrapolator

from InDetRecExample import TrackingCommon

trigBphysFitter = Trk__TrkVKalVrtFitter("TrigBphysFitter",
                                        FirstMeasuredPoint = False,
                                        MakeExtendedVertex = True,
                                        #OutputLevel  = 2,
                                        Extrapolator = TrackingCommon.getInDetExtrapolator(),
                                        )
ToolSvc += trigBphysFitter
ToolSvc += TrigBphysHelperUtilsTool(VertexFitterTool = ToolSvc.TrigBphysFitter,
                                    #OutputLevel=2
                                    )
                                    
# fitter and helper tool with legacy configuration
trigBphysFitterLegacy = Trk__TrkVKalVrtFitter("TrigBphysFitterLegacy",
                                              #OutputLevel  = 2,
                                              )
ToolSvc += trigBphysFitterLegacy
trigBphysHelperUtilsToolLegacy = TrigBphysHelperUtilsTool("TrigBphysHelperUtilsToolLegacy",
                                                          VertexFitterTool = ToolSvc.TrigBphysFitterLegacy,
                                                          #OutputLevel  = 2,
                                                          ) #,
ToolSvc += trigBphysHelperUtilsToolLegacy

#print ToolSvc.TrigBphysFitter
#print ToolSvc.TrigBphysFitterLegacy
#print ToolSvc.TrigBphysHelperUtilsTool
#print ToolSvc.TrigBphysHelperUtilsToolLegacy


