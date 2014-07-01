# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CaloTrackingGeometry.CaloTrackingGeometryConf import CaloSurfaceBuilder
# provide generic configuration of CaloSurfaceBuilder tool
            
class CaloSurfaceBuilderBase ( CaloSurfaceBuilder )  :

    # explicit list of additional data member to catch typo
    __slots__ = ( )
    
    def __init__(self, name="CaloSurfaceBuilderBaseUNSET",depth="CDTBunset"):        
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( name+'(Base)::__init__ ' )
        CaloSurfaceBuilder.__init__( self,name) # call autogen base class constructor

        mlog.info("Setting up CaloSurfaceBuilder/"+name+" with depth "+depth)   

        if name=="CaloSurfaceBuilderBaseUNSET":
            raise RuntimeError, "CaloSurfaceBuilderBaseUNSET should not be called with default name"            

        if depth=="CDTBunset":
            mlog.info( "depth not explictly set. OK only for copy")
            return
        
        from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
        theCaloDepthTool=CaloDepthToolFactory(depth=depth)

        
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloDepthTool

        self.CaloDepthTool=theCaloDepthTool

# factory : autogenerates the name
# usage :
# from CaloSurfaceBuilder.CaloSurfaceBuilderBase import CaloSurfaceBuilderFactory
# theTool=CaloSurfaceBuilderFactory(depth="showerdefault")

def CaloSurfaceBuilderFactory( depth ):
# build the tool name by appending depth value
   return CaloSurfaceBuilderBase( "CaloSurfaceBuilder"+depth, depth=depth ) 

