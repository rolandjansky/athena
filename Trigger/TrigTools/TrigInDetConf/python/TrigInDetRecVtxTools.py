# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
#
# the configuration of tools for vertexing
#
"""

___author___ = "Jiri Masik"
___version___ = "$Id: TrigInDetRecVtxTools.py 598141 2014-05-21 04:30:45Z masik $"


from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags



#
# load linearized track factory
#
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator

InDetTrigLinFactory = Trk__FullLinearizedTrackFactory(name              = "InDetTrigFullLinearizedTrackFactory",
                                                      Extrapolator      = InDetTrigExtrapolator,
                                                      )
ToolSvc += InDetTrigLinFactory
if (InDetTrigFlags.doPrintConfigurables()):
  print      InDetTrigLinFactory


#
# VxEDMCnv
#
from TrkVxEdmCnv.TrkVxEdmCnvConf import Trk__VxCandidateXAODVertex
InDetTrigVxEdmCnv = Trk__VxCandidateXAODVertex(name="TrigVertexInternalEdmFactory")
InDetTrigVxEdmCnv.LinearizedTrackFactory=InDetTrigLinFactory
ToolSvc += InDetTrigVxEdmCnv

if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigVxEdmCnv
