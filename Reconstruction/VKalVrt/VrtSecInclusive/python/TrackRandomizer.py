# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ToolSvc
#
from VrtSecInclusiveConf import VKalVrtAthena__TrackRandomizer

class TrackRandomizer ( VKalVrtAthena__TrackRandomizer )  :

    def __init__(self, name="TrackRandomizer"):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'TrackRandomizer::__init__ ' )
        mlog.info("entering")

        VKalVrtAthena__TrackRandomizer.__init__( self, name )

