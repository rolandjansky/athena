# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# specifies LArRawChannels getting

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured

class LArRawChannelGetter_DigiHSTruth ( Configured )  :
    _outputType = "LArRawChannelContainer"
    _output = { _outputType : "LArRawChannels_DigiHSTruth" } # do not want to list all the IDC
        
    def configure(self):
        mlog = logging.getLogger( 'Py:LArRawChannelGetter_DigiHSTruth::configure %s:' % self.__class__ )

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        from LArROD.LArRODFlags import larRODFlags

        from AthenaCommon.GlobalFlags import globalflags
        # This should never be run in data
        if globalflags.DataSource()=='data' or larRODFlags.forceIter() :
            return True

        else:
            # MC Case
            try:
                from LArROD.LArRODConfig import getLArRawChannelBuilder_DigiHSTruth
                theLArRawChannelBuilder_DigiHSTruth = getLArRawChannelBuilder_DigiHSTruth()
                topSequence += theLArRawChannelBuilder_DigiHSTruth
            except Exception:
                import traceback
                mlog.error(traceback.format_exc())
                mlog.error("Failed to retrieve LArRawChannelBuilder_DigiHSTruth. Quit")
                return False

        return True

# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def outputTypeKey(self):
        return str(self.outputType()+"#"+self.outputKey())
