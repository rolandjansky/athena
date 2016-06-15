# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

trigBphysFitter = Trk__TrkVKalVrtFitter("TrigBphysFitter",
                                        FirstMeasuredPoint = False,
                                        MakeExtendedVertex = True,
                                        #OutputLevel  = 2,
                                        Extrapolator = "Trk::Extrapolator/InDetExtrapolator",
                                        )
ToolSvc += trigBphysFitter
ToolSvc += TrigBphysHelperUtilsTool(VertexFitterTool = ToolSvc.TrigBphysFitter) #,
                                    #OutputLevel=2)

#print ToolSvc.TrigBphysHelperUtilsTool


