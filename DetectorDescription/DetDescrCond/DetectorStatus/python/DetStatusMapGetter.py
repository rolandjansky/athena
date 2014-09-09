# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# specified DetStatusMap output
#
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class DetStatusMapGetter ( Configured )  :
    _outputType = "DetStatusMap"
    _output = { _outputType : "DetectorStatus" }

        
    def configure(self):
        mlog = logging.getLogger( 'DetStatusMapGetter::configure :' )
        mlog.info ('entering')

                
        

        # now configure the algorithm
        try:        
            from DetectorStatus.DetectorStatusConf import DetStatusAlg               
        except:
            mlog.error("could not import DetStatusAlg")
            print traceback.format_exc()
            return False

        # sets up cond db
        # still use old include for time being
        from AthenaCommon.Include import include        
        include("DetectorStatus/DetStatusSvc_CondDB.py")


        # instantiate algorithm with default properties
        theDetStatusAlg=DetStatusAlg(Write=True)
        self._DetStatusAlgHandle = theDetStatusAlg ;


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        # output both in ESD and AOD
        objKeyStore.addStreamESD(self.outputType(),self.outputKey())
        objKeyStore.addStreamAOD(self.outputType(),self.outputKey())        


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from __main__ import topSequence
        topSequence += theDetStatusAlg ;
        
        return True

    def detStatusAlgHandle(self):
        return self._DetStatusAlgHandle

# only one algorithm : generic name is unambiguous
    def algHandle(self):
        return self._DetStatusAlgHandle

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType


