# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# AtlasTrackSummaryTool module
#
# it inherits from Trk__TrackSummaryTool and uses
#
######################################################

# import the include statement
from AthenaCommon.Include import Include, IncludeError, include

# import the ToolSvc
from AthenaCommon.AppMgr import ToolSvc

# import detflags
from AthenaCommon.DetFlags import DetFlags

# import the Extrapolator configurable
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool

# define the class
class AtlasTrackSummaryTool( Trk__TrackSummaryTool ):
    # constructor
    def __init__(self,name = 'AtlasTrackSummaryTool'):
        from AthenaCommon.AppMgr import ToolSvc        

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
        # Loading Configurable HoleSearchTool
        #
        from InDetRecExample.InDetJobProperties import InDetFlags
        from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
        AtlasHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "AtlasHoleSearchTool",
                                                              Extrapolator = AtlasExtrapolator,
                                                              usePixel      = DetFlags.haveRIO.pixel_on(),
                                                              useSCT        = DetFlags.haveRIO.SCT_on(),
                                                              checkBadSCTChip=InDetFlags.checkDeadElementsOnTrack())
        
        if InDetFlags.doSLHC():
          AtlasHoleSearchTool.ITkGeometry = True
        
        ToolSvc += AtlasHoleSearchTool
        #print      AtlasHoleSearchTool
        
        #
        # Configrable version of loading the AtlasTrackSummaryHelperTool
        #
        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        if DetFlags.haveRIO.TRT_on():
            AtlasTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(
                name         = "AtlasTrackSummaryHelperTool",
                HoleSearch   = AtlasHoleSearchTool,
                AssoTool     = AtlasPrdAssociationTool,
                DoSharedHits = False)
        else:
            AtlasTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(
                name         = "AtlasTrackSummaryHelperTool",
                HoleSearch   = AtlasHoleSearchTool,
                AssoTool     = AtlasPrdAssociationTool,
                DoSharedHits = False,
                TRTStrawSummarySvc = "")
            
        if InDetFlags.doSLHC():
          AtlasTrackSummaryHelperTool.ITkGeometry = True
            
        ToolSvc += AtlasTrackSummaryHelperTool
        #print      AtlasTrackSummaryHelperTool                                                                   
        
        #
        # Configurable version of TrkTrackSummaryTool
        #
        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        AtlasTrackSummaryTool = Trk__TrackSummaryTool(
                                                     )
        # call the base class constructor
        Trk__TrackSummaryTool.__init__(self, name = name,
                                             InDetSummaryHelperTool = AtlasTrackSummaryHelperTool,
                                             doHolesOnTrack    = True,
                                             doSharedHits      = AtlasTrackSummaryHelperTool.DoSharedHits
                                      )

