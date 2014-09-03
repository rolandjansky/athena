# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from JetRec.JetToolSupport import jtm
import JetRec.JetRecStandardTools
from JetRecTools.JetRecToolsConf import JetTrackSelectionTool , TrackVertexAssociationTool, MissingCellListTool, TrackPseudoJetGetter


# ------------------------------------------------
# Bad/Missing Cells list creation
jtm += MissingCellListTool("MissingCellListTool",
                           AddBadCells = True,
                           )
# ------------------------------------------------

# Bad channel correction tools
def badChanInputHistoHelper():
    # keep it as a function for now : might be easier to deal with when
    # root-only config will be needed.
    # new setup for THistService    
    dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
    dataPathList.insert(0, os.curdir)
    from AthenaCommon.Utils.unixtools import FindFile
    RefFileName = FindFile( "JetBadChanCorrTool.root" ,dataPathList, os.R_OK )
 

    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
    ServiceMgr.THistSvc.Input += ["JetBadChanCorrTool DATAFILE=\'%s\' OPT=\'READ\'" % RefFileName]
    badChanInputHistoHelper.called = True


badChanInputHistoHelper()
from JetMomentTools.JetMomentToolsConf import JetBadChanCorrTool
jtm += JetBadChanCorrTool("JetBadChanCorrTool", MissingCellMap="MissingCaloCellsMap")

# ------------------------------------------------
# ------------------------------------------------


# ------------------------------------------------
# track tools
# ------------------------------------------------

# ------------------------------------------------
## # tool building the selected tracks
## jtm += JetTrackSelectionTool("JetTrackSelectionBuilder",
##                              InputContainer = "InDetTrackParticles",
##                              OutputContainer="JetSelectedTracks",
##                              Selector=jtm.jet_tracksel)

## # tool building the TrackVertexAssociation
## jtm += TrackVertexAssociationTool("TrackVertexAssociationTool",
##                                   TrackParticleContainer = "InDetTrackParticles" ,
##                                   TrackVertexAssociation="JetTrackVtxAssoc",
##                                   VertexContainer="PrimaryVertices",
##                                   MaxTransverseDistance = 1.,
##                                   MaxLongitudinalDistance =1.)

## # track pseudojet getters
## jtm += TrackPseudoJetGetter( "TrackPseudoJetGetter",
##                              InputContainer = "InDetTrackParticles",
##                              Label = "Tracks",
##                              OutputContainer = "PseudoJetTracks",
##                              TrackVertexAssociation = jtm.TrackVertexAssociationTool.TrackVertexAssociation,
##                              SkipNegativeEnergy = True,
##                              GhostScale = 0.0
##                              )


## jtm += TrackPseudoJetGetter( "GhostTrackPseudoJetGetter",   InputContainer = "InDetTrackParticles",
##                              Label = "GhostTracks",
##                              OutputContainer = "GhostPseudoJetTracks",
##                              TrackVertexAssociation = jtm.TrackVertexAssociationTool.TrackVertexAssociation,
##                              SkipNegativeEnergy = True,
##                              GhostScale = 1e-50,
##                              )


## # ------------------------------------------------

