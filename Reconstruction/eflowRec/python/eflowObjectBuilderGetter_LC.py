# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
from AthenaCommon.Constants import * # Loads DEBUG INFO etc..
from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class
from AthenaCommon.Include import include

class eflowObjectBuilderGetter_LC ( Configured )  :

    _outputType = "xAOD::PFOContainer"
    _output = { _outputType : "chargedJetETMiss_LC_eflowRec",   _outputType : "neutralJetETMiss_LC_eflowRec"}

    def configure(self):

        mlog = logging.getLogger( 'eflowObjectBuilderGetter_LC:configure :' )
        mlog.info("Output="+self.outputKey() ) # prints an info message

        from eflowRec.eflowObjectBuilderSetup import setup_eflowObjectBuilder
        return setup_eflowObjectBuilder(self, "LC", mlog)

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def eflowObjectBuilderHandle(self):
        return self._eflowObjectBuilderHandle
