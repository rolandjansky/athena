# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from RecExConfig.Configured import Configured # import base class

class eflowObjectBuilderGetter_HLLHC ( Configured )  :

    _outputType = "xAOD::PFOContainer"
    _output = { _outputType : "chargedJetETMiss_eflowRec",   _outputType : "neutralJetETMiss_eflowRec"}

    def configure(self):

        mlog = logging.getLogger( 'eflowObjectBuilderGetter_HLLHC:configure :' )
        mlog.info("Output="+self.outputKey() ) # prints an info message

        from eflowRec.eflowObjectBuilderSetup import setup_eflowObjectBuilder
        return setup_eflowObjectBuilder(self, "EM_HLLHC", mlog)

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def eflowObjectBuilderHandle(self):
        return self._eflowObjectBuilderHandle
