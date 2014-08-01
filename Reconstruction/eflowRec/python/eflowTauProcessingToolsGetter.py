# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured # import base class

from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back


class eflowTauProcessingToolsGetter(Configured):

    _outputType = "eflowObjectContainer"
    _output = { _outputType : "eflowObjects_tauModeTest" }

    def configure(self):

        mlog = logging.getLogger( 'eflowTauProcessingTools:configure :' )
        mlog.info("Output="+self.outputKey() ) # prints an info message

        try:
            from eflowRec.eflowRecConf import eflowTauProcessingTools
            TauProcessingTools = eflowTauProcessingTools("eflowTauProcessingTools")
        except:
            mlog.error("could not import eflowRec.TauProcessingTools")
            print traceback.format_exc()
            return False

        self._TauProcessingToolsHandle = TauProcessingTools

        try:
            from eflowRec.eflowTauToolDefault import eflowTauToolDefault
            TauTool = eflowTauToolDefault()
        except:
            mlog.error("could not import eflowRec.eflowTauToolDefault")
            print traceback.format_exc()
            return False

        TauProcessingTools.PrivateToolList=[TauTool]

        try:
            from eflowRec.eflowTauLCCalibToolDefault import eflowTauLCCalibToolDefault
            TauLCCalibTool = eflowTauLCCalibToolDefault()
        except:
            mlog.error("could not import eflowRec.eflowTauLCCalibToolDefault")
            print traceback.format_exc()
            return False

        TauProcessingTools.PrivateToolList+=[TauLCCalibTool]

        try:
            from eflowRec.eflowRecConf import eflowTauPi0TaggerTool
            eflowTauPi0Tool = eflowTauPi0TaggerTool()
        except:
            mlog.error("could not import eflowRec.eflowTauPi0TaggerToolDefault")
            print traceback.format_exc()
            return False

        TauProcessingTools.PrivateToolList+=[eflowTauPi0Tool]

        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addTransient(self.outputType(),self.outputKey())
        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += TauProcessingTools;

        return True;

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def eflowTauProcessingToolsHandle(self):
        return self._eflowTauProcessingToolsHandle
