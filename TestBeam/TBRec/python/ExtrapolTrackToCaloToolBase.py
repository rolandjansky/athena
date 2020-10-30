# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TBRec.TBRecConf import ExtrapolTrackToCaloTool

# to be fixed  more try catch, use more fully configured tool
            
class ExtrapolTrackToCaloToolBase ( ExtrapolTrackToCaloTool )  :

    def __init__(self, name="ExtrapolTrackToCaloToolBaseUNSET",depth="ETTCTunset",straightLine=False):
   
        ExtrapolTrackToCaloTool.__init__( self,name) # call base class constructor

        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( name+'(Base)::__init__ ' )
        mlog.info("entering")


        if name=="ExtrapolTrackToCaloToolBaseUNSET":
            mlog.info ("name should be explicitly set")
            return

        if depth=="ETTCTunset":
            mlog.info ("default depth value. OK only if copy")
            # depth=self._depth
            # raise RuntimeError,"depth should be explicitly set"
            return
 
        # should instantioate CaloDetDescMger
        # should instantiate calo ID helper
        # should instantiate TBCaloCoordinate tool

        
        from CaloTrackingGeometry.CaloSurfaceBuilderBase import CaloSurfaceBuilderFactory
        theCaloSurfaceBuilder=CaloSurfaceBuilderFactory(depth)

        # public tool
        from AthenaCommon.AppMgr import ToolSvc

        ToolSvc+=theCaloSurfaceBuilder
        self.CaloSurfaceBuilder=theCaloSurfaceBuilder

        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
 
        from AthenaCommon.AppMgr import ToolSvc

        if not straightLine:
            theExtrapolator=AtlasExtrapolator()
        else:    
            theExtrapolator=AtlasExtrapolator("ATLASExtrapolatorStraightLine")
            from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator as Propagator
            SlPropagator =  Propagator(name='TTC_SlPropagator')
            ToolSvc += SlPropagator
            theExtrapolator.Propagators = [ SlPropagator ]
            
            
        # need to add to ToolSvc before putting in ToolHandle
        ToolSvc+=theExtrapolator

        self.Extrapolator=theExtrapolator



# factory : autogenerates the name
# usage :
# from TBRec.ExtrapolTrackToCaloBase import ExtrapolTrackToCaloToolFactory
# theTool=ExtrapolTrackToCaloToolFactory(depth="showerdefault",straightLine=False)
def ExtrapolTrackToCaloToolFactory( depth,straightLine=False ):
    # build the tool name by appending  depth valure
    if straightLine:
        straightName="Straight"
    else:
        straightName=""
    return ExtrapolTrackToCaloToolBase( "ExtrapolTrackToCaloTool"+depth+straightName, depth=depth,straightLine=straightLine ) 
