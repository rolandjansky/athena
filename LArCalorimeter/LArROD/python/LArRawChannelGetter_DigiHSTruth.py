# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
        from AthenaCommon.AppMgr import ToolSvc


        # get LArDigitGetter in MC case
        #from AthenaCommon.DetFlags import DetFlags
        #if DetFlags.digitize.LAr_on() :
        #    try:
        #        from LArDigitization.LArDigitGetter import LArDigitGetter
        #        theLArDigitGetter = LArDigitGetter()
        #    except Exception as configException:
        #        print configException
        #        mlog.error("could not get handle to LArDigitGetter Quit")
        #        return False
        #    if not theLArDigitGetter.usable():
        #        mlog.error("LArDigitGetter unusable. Quite")
        #        return False

        from LArROD.LArRODFlags import larRODFlags

        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource()=='data' or larRODFlags.forceIter() :
            return True

        else:

            # MC Case
            try:
                from AthenaCommon import CfgGetter
                #topSequence += CfgGetter.getAlgorithm("LArRawChannelBuilder", tryDefaultConfigurable=True)
                #theLArRawSignalChannelBuilder=LArRawChannelBuilderDriver("LArRawSignalChannelBuilder")
                #theLArRawSignalChannelBuilder = CfgGetter.getAlgorithm("LArRawChannelBuilder", "LArRawSignalChannelBuilder")
                #theLArRawSignalChannelBuilder.DataLocation    = "LArSignalDigitContainer"
                #theLArRawSignalChannelBuilder.LArRawChannelContainerName = "LArRawSignalChannels"
                #topSequence += theLArRawSignalChannelBuilder
                from LArRODConfig import getLArRawChannelBuilder_DigiHSTruth
                theLArRawChannelBuilder_DigiHSTruth = getLArRawChannelBuilder_DigiHSTruth()
                topSequence += theLArRawChannelBuilder_DigiHSTruth
            except Exception as cfgException:
                print cfgException
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
