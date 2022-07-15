# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

######################################################
# AtlasTrackSummaryTool module
#
# it inherits from Trk__TrackSummaryTool and uses
#
######################################################

# import detflags
from AthenaCommon.DetFlags import DetFlags

# import the Extrapolator configurable
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool

# define the class
class AtlasTrackSummaryTool( Trk__TrackSummaryTool ):
    # constructor
    def __init__(self,name = 'AtlasTrackSummaryTool'):
        from AthenaCommon.AppMgr import ToolSvc        
        import InDetRecExample.TrackingCommon as TrackingCommon

        #
        # Setup Atlas Extrapolator
        #
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        AtlasExtrapolator = AtlasExtrapolator()
        ToolSvc += AtlasExtrapolator
        
        #
        # Setup Association Tool
        #
        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        AtlasPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(
                                                                              name                           = "AtlasPrdAssociationTool",
                                                                              PixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap"
                                                                             )
        ToolSvc += AtlasPrdAssociationTool
        #print      AtlasPrdAssociationTool

        
        # 
        # Setup Boundary Check Tool
        #
        AtlasBoundaryCheckTool = TrackingCommon.getInDetBoundaryCheckTool(name="AtlasBoundaryCheckTool")
    
        #
        # Loading Configurable HoleSearchTool
        #
        from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
        AtlasHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "AtlasHoleSearchTool",
                                                              Extrapolator = AtlasExtrapolator,
                                                              BoundaryCheckTool=AtlasBoundaryCheckTool
                                                              )
        ToolSvc += AtlasHoleSearchTool
        #print      AtlasHoleSearchTool
        
        #
        # Configrable version of loading the AtlasTrackSummaryHelperTool
        #
        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        AtlasTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(
            name           = "AtlasTrackSummaryHelperTool",
            HoleSearch     = AtlasHoleSearchTool,
            AssoTool       = AtlasPrdAssociationTool,
            DoSharedHits   = False,
            useTRT         = DetFlags.haveRIO.TRT_on()
        )

        ToolSvc += AtlasTrackSummaryHelperTool
        #print      AtlasTrackSummaryHelperTool                                                                   
        
        #
        # Configurable version of TrkTrackSummaryTool
        #
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool

        # call the base class constructor
        Trk__TrackSummaryTool.__init__(self, name = name,
                                             InDetSummaryHelperTool = AtlasTrackSummaryHelperTool,
                                             doHolesOnTrack    = True,
                                             doSharedHits      = AtlasTrackSummaryHelperTool.DoSharedHits
                                      )

