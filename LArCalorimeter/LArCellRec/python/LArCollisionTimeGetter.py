# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file LArCellRec/python/LArCollisionTimeGetter.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief Configure algorithm to calculate LAr collision time variables.
#


from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence

class LArCollisionTimeGetter ( Configured )  :
    _outputType = "LArCollisionTime"
    _output = { _outputType : "LArCollisionTime" }


    def __init__ (self,
                  seq = AlgSequence(),
                  *args, **kw):
        self.seq = seq
        Configured.__init__ (self, *args, **kw)
        return

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'LArNoisyROSummaryGetter::configure:' )
        mlog.info ('entering')

        import traceback
        try:
            from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
            CaloNoiseCondAlg()

            from LArCellRec.LArCellRecConf import LArCollisionTimeAlg
            from AthenaCommon.GlobalFlags  import globalflags
            self._handle = \
              LArCollisionTimeAlg("LArCollisionTimeAlg",
                                  isMC = globalflags.DataSource != 'data',
                                  cutIteration=False)

            # register output in objKeyStore
            from RecExConfig.ObjKeyStore import objKeyStore
            objKeyStore.addTransient (self.outputType(),self.outputKey())

            self.seq += self._handle
        except Exception:
            mlog.error ("Error configuring LArCollisionTimeAlg.")
            traceback.print_exc()

        return True


    def handle (self):
        return self._handle

    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType
