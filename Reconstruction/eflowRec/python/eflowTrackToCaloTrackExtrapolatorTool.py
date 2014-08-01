# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Include import include

# import the base class
from eflowRec.eflowRecConf import eflowTrackToCaloTrackExtrapolatorTool

class eflowTrackToCaloTrackExtrapolatorToolDefault(eflowTrackToCaloTrackExtrapolatorTool) :

    def __init__(self, name="eflowTrackToCaloTrackExtrapolatorToolDefault"): 
        # call base class constructor
        eflowTrackToCaloTrackExtrapolatorTool.__init__( self, "eflowTrackToCaloTrackExtrapolatorTool" )  

        mlog = logging.getLogger( name+'::__init__ ' )
        mlog.info("entering")

        try:
            from TrackToCalo.ExtrapolateToCaloToolBase import ExtrapolateToCaloToolFactory
            eflowTrackToCalo = ExtrapolateToCaloToolFactory(depth="middle",straightLine=False)
        except:
            mlog.error("could not import ExtrapolTrackToCaloToolFactory from TrackToCalo.ExtrapolTrackToCaloToolBase")
            print traceback.format_exc()
            return False

        try:
            from AthenaCommon.AppMgr import ToolSvc
        except:
            mlog.error("could not import ToolSvc from AthenaCommon.AppMgr")
            print traceback.format_exc()
            return False

        ToolSvc+=eflowTrackToCalo

        self.TrackToCalo=eflowTrackToCalo

        
