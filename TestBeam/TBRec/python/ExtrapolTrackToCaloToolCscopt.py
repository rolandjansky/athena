# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from TBRec.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolBase

class ExtrapolTrackToCaloToolCscopt ( ExtrapolTrackToCaloToolBase )  :

    def __init__(self):
        ExtrapolTrackToCaloToolBase.__init__( self) # call base class constructor        
        pass

    def __init__(self, name="ExtrapolTrackToCaloToolCscopt"):        
        mlog = logging.getLogger( name+'__init__ ' )
        mlog.info("entering")
        mlog.warning("ExtrapolTrackToCaloToolCscopt obsolete !")
        mlog.warning("from TBRec.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolFactory")
        mlog.warning("theTool=ExtrapolTrackToCaloToolFactory(depth=cscopt)")
        ExtrapolTrackToCaloToolBase.__init__( self,name,depth="cscopt") # call base class constructor


