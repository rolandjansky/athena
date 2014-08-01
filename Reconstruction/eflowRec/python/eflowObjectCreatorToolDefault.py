# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Include import include
#from AthenaCommon.GlobalFlags import *
from AthenaCommon.JobProperties import jobproperties
import traceback
import sys
# import the base class
from eflowRec.eflowRecConf import eflowObjectCreatorTool

class eflowObjectCreatorToolDefault(eflowObjectCreatorTool) :

    def __init__(self, name="eflowObjectCreatorToolDefault"): 

        eflowObjectCreatorTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowObjectCreatorToolDefault::__init__ ' )
        mlog.info("entering")

        from eflowRec.eflowRecFlags import jobproperties
        gmFlag=""# default set to "golden1" for goldenMode. In the end this should come from this flag jobproperties.eflowRecFlags.goldenModeString()
        self.goldenModeString=gmFlag
        print "Configuring eflowObjectCreatorTool with goldenModeString: ",gmFlag
