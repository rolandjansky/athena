# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies a specific SimpleCookedObjMaker instance

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured

class SimpleCookedObjGetter ( Configured )  :
    _outputType = "SimpleCookedObj"
    _output = { _outputType : "SimpleCookedObjBlaBlaDict" }

        
    def configure(self):
        mlog = logging.getLogger( 'Py:SimpleCookedObjGetter::configure %s:' % self.__class__ )


        # work indeed
        # from __main__ import include
        # include("RecExAlgs/SimpleCookedObjMaker_jobOptions.py")

        # get handle to upstream object

        try: 
            from RecExAlgs.SimpleRawObjGetter import SimpleRawObjGetter
        except ImportError,err:
            mlog.error("could not import SimpleRawObjGetter. Quit")
            return False
            
        theSimpleRawObjGetter=SimpleRawObjGetter()

        if not theSimpleRawObjGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("SimpleRawObjGetter unusable. Quit.")
                return False
            else:
                mlog.error("SimpleRawObjGetter unusable. Continue nevertheless")

                
        # get handle to tools
        

        # now configure the algorithm, part of this could be done in a separate class
        # cannot have same name
        try:
            from RecExAlgs.RecExAlgsConf import SimpleCookedObjMaker
        except ImportError:
            mlog.error("could not import SimpleCookedObjMaker. Quit")
            return False

        theSimpleCookedObjMaker=SimpleCookedObjMaker()
        self._SimpleCookedObjMakerHandle = theSimpleCookedObjMaker ;
        theSimpleCookedObjMaker.CookedMessage= "From SimpleCookedObjGetter.py"
        #

        # sets output key  
        # these two lines are completely equivalent
        #self.SimpleCookedObjMakerHandle().CookedObjOutputName=self.outputKey()
        theSimpleCookedObjMaker.CookedObjOutputName=self.outputKey()        


        # register output in objKeyStore
        # should eventually specify there that object to be written in a stream
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addStreamESD(self.outputType(),self.outputKey())
        # if only transient store
        # objKeyStore.addTransient(self.outputType(),self.outputKey())


        # can alter upstream alg property here, not recommended
        # theSimpleRawObjGetter.SimpleRawObjMakerHandle().RawMessage= "from SimpleCookedObjGetter"
        
        theSimpleCookedObjMaker.RawObjNames = [ theSimpleRawObjGetter.outputKey()]

        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.SimpleCookedObjMakerHandle()

        
        return True

    def SimpleCookedObjMakerHandle(self):
        return self._SimpleCookedObjMakerHandle

   
# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def outputTypeKey(self):
        return str(self.outputType()+"#"+self.outputKey())


