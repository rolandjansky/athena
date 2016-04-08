# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies a specific SimpleRawObjMaker instance

from AthenaCommon.Logging import logging
from RecExConfig.Configured import *


class SimpleRawObjGetter ( Configured )  :
    _outputType = "SimpleRawObj"
    # initialisation syntax {"typea":["key1", "key2"], "typeb":"key"}
    _output =  { _outputType :"SimpleRawObjBlaBlaDict" }


    def configure(self):
        # should be only one
        mlog = logging.getLogger( 'Py:SimpleRawObjGetter::configure %s:' % self.__class__ )



        # now configure the algorithm, part of this could be done in a separate file, a Configurable python class for example

        try:
            from RecExAlgs.RecExAlgsConf import SimpleRawObjMaker
        except ImportError,err:
            mlog.error("could not import SimpleRawObjMaker:")
            mlog.error(err)
            return False

        # create alg instance
        theSimpleRawObjMaker=SimpleRawObjMaker()
        self._SimpleRawObjMakerHandle = theSimpleRawObjMaker 
        # sets output key  


        theSimpleRawObjMaker.RawObjOutputName=self.outputKey()

        theSimpleRawObjMaker.RawMessage=" from SimpleRawObjGetter "
        # this line is equivalent to the previous
        #self.SimpleRawObjMakerHandle().RawMessage=" from SimpleRawObjGetter "     
        
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addTransient(self.outputType(),self.outputKey())

        #anotherSimpleRawObjMaker=SimpleRawObjMaker()

        mlog.info(" now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.SimpleRawObjMakerHandle()

        # this still work
        #theAlg.RawMessage=" from SimpleRawObjGetter with theAlg after topseq"             

        mlog.info(" leaving configure")
        
        return True


    def SimpleRawObjMakerHandle(self):
        return self._SimpleRawObjMakerHandle
   
# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def outputTypeKey(self):
        return str(self.outputType()+"#"+self.outputKey())



