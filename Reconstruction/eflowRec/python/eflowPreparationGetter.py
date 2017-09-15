# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from RecExConfig.Configured import Configured # import base class

class eflowPreparationGetter ( Configured )  :  # python way to define inheritance

    _outputType = "eflowCaloObjectContainer"
    _output = { _outputType : "eflowCaloObjects01" }

    def configure(self):

        mlog = logging.getLogger( 'eflowPreparationGetter:configure :' )
        mlog.info("Output="+self.outputKey() ) # prints an info message

        from eflowRec.eflowCaloSetup import setup_eflowCaloObjectCreator
        return setup_eflowCaloObjectCreator(self, "EM", mlog)

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def eflowPreparationHandle(self):
        return self._eflowPreparationHandle
