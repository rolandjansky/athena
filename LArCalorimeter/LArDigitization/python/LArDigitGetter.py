# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Getter for LArDigit from LArHit for MC digitization

from AthenaCommon.Logging import logging
import traceback
from RecExConfig.Configured import Configured

class LArDigitGetter (Configured) :
    _outputType = "LArDigitContainer"
    _output = { _outputType : "LArDigitContainer_MC" }

    def configure(self):
        mlog = logging.getLogger( 'LArDigitGetter.py::configure :' )
        mlog.info ('entering')

        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()

        try:
            from AthenaCommon import CfgGetter
            # if using new scheme for pileup
            from Digitization.DigitizationFlags import digitizationFlags
            if digitizationFlags.doXingByXingPileUp():
                # Defined in LArDigitizationConfig.py
                job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("LArPileUpTool", checkType=True) ]
                job.PileUpToolsAlg.PileUpTools["LArPileUpTool"].DigitContainer = self.outputKey()
                job.PileUpToolsAlg.PileUpTools["LArPileUpTool"].DigitContainer_DigiHSTruth = self.outputKey_DigiHSTruth()
                from RecExConfig.RecFlags import rec
                job.PileUpToolsAlg.PileUpTools["LArPileUpTool"].DoDigiTruthReconstruction = rec.doDigiTruth()
            else:
                # Defined in LArDigitizationConfig.py
                print "call CfgGetter for digitmaker1 "
                job += CfgGetter.getAlgorithm("digitmaker1", tryDefaultConfigurable=True)
                job.digitmaker1.LArPileUpTool.DigitContainer = self.outputKey()
                job.digitmaker1.LArPileUpTool.DigitContainer_DigiHSTruth = self.outputKey_DigiHSTruth()
                from RecExConfig.RecFlags import rec
                job.digitmaker1.LArPileUpTool.DoDigiTruthReconstruction = rec.doDigiTruth()
                # if pileup or overlay
                from AthenaCommon.DetFlags import DetFlags
                from LArDigitization.LArDigitizationConfig import isOverlay
                if DetFlags.pileup.LAr_on() or isOverlay():
                    from AthenaCommon.AppMgr import ServiceMgr
                    # Defined in LArDigitizationConfig.py
                    ServiceMgr.PileUpMergeSvc.Intervals += [ CfgGetter.getPrivateTool("LArRangeEM", checkType=True) ]
                    ServiceMgr.PileUpMergeSvc.Intervals += [ CfgGetter.getPrivateTool("LArRangeHEC", checkType=True) ]
                    ServiceMgr.PileUpMergeSvc.Intervals += [ CfgGetter.getPrivateTool("LArRangeFCAL", checkType=True) ]
        except Exception as configException:
            print configException
            print "ERROR Problem with configuration"
        
        return True

    def outputKey(cls):
        return cls._output[cls._outputType]

    def outputType(cls):
        return cls._outputType

    def outputTypeKey(self):
        return str(self.outputType()+"#"+self.outputKey())
