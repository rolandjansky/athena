# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# specifies LArSCell getting

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured

class LArSCellGetter ( Configured )  :
    #_outputType = "CaloCellContainer"
    #_output = { _outputType : "SCell" } # do not want to list all the IDC
        
    def configure(self):
        mlog = logging.getLogger( 'Py:LArSCellGetter::configure %s:' % self.__class__ )

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()


        # get LArDigitGetter in MC case
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.digitize.LAr_on() :
            try:
                from LArL1Sim.LArSCL1Getter import LArSCL1Getter
                theLArSCL1Getter = LArSCL1Getter()
            except Exception:
                import traceback
                mlog.error(traceback.format_exc())
                mlog.error("could not get handle to LArDigitGetter Quit")
                return False
            if not theLArSCL1Getter.usable():
                mlog.error("LArSCL1Getter unusable. Quite")
                return False

# Only MC case

        from LArROD.LArSuperCellBuilderAlgDefault import LArSuperCellBuilderAlgDefault,LArSuperCellBCIDAlgDefault
        topSequence+=LArSuperCellBuilderAlgDefault()
        topSequence+=LArSuperCellBCIDAlgDefault()


        return True

# would work only if one output object type
    def LArSCellBuilder(self):
        return self._LArSCellBuilder
    def outputKey(cls):
        return cls._output[cls._outputType]

    def outputType(cls):
        return cls._outputType

    def outputTypeKey(self):
        return str(self.outputType()+"#"+self.outputKey())
