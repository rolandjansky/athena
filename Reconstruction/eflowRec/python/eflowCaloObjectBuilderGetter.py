# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
from AthenaCommon.Constants import * # Loads DEBUG INFO etc..
from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class

class eflowCaloObjectBuilderGetter ( Configured )  :  # python way to define inheritance

    _outputType = "eflowCaloObjectContainer"
    _output = { _outputType : "eflowCaloObjects01" }

    def configure(self):

        mlog = logging.getLogger( 'eflowCaloObjectBuilderGetter:configure :' )
        mlog.info("Output="+self.outputKey() ) # prints an info message

        from eflowRec.eflowCaloSetup import setup_eflowCaloObjectBulder
        return setup_eflowCaloObjectBulder(self, "EM", mlog)

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def eflowCaloObjectBuilderHandle(self):
        return self._eflowCaloObjectBuilderHandle
