# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ToolSvc
#
from VrtSecInclusiveConf import VKalVrtAthena__TrackSlimmer

class TrackSlimmer ( VKalVrtAthena__TrackSlimmer )  :

    def __init__(self, name="TrackSlimmer"):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'TrackSlimmer::__init__ ' )
        mlog.info("entering")

        VKalVrtAthena__TrackSlimmer.__init__( self, name )

