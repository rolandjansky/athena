# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
from AthenaCommon.Constants import * # Loads DEBUG INFO etc..
from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class
from AthenaCommon.Include import include

class eflowCaloObjectBuilderGetter ( Configured )  :

    def configure(self):

        mlog = logging.getLogger( 'eflowCaloObjectBuilderGetter:configure :' )

        from eflowRec.eflowCaloObjectBuilder import setup_eflowObjectBuilderTools
        return setup_eflowObjectBuilderTools(self, "EM_HLLHC", mlog)

    def eflowObjectBuilderToolsHandle(self):
        return self._eflowObjectBuilderToolsHandle

   
