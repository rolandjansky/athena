# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies egamma"standard"
from AthenaCommon.Logging import logging

from RecExConfig.Configured import Configured


class ZdcRawChannelGetter ( Configured ) :
    _outputType = "ZdcRawChannelCollection"

    _output = { _outputType: ["ZdcRawChannelCollection" ]}


    def preconfigure (self):
      mlog = logging.getLogger ('ZdcRawChannelGetter.py::preconfigure:')
      mlog.info('entering')

      preconfigureBase = super (ZdcRawChannelGetter,self).preconfigure() 

      
      if not preconfigureBase:
          return False

      #mlog = logging.getLogger( 'Configured::preconfigure:%s:' % self.__class__.__name__.replace( ".", '_' )  )
      #mlog.debug("Output= %s" % self.output() )
      #
      #if self.checkExistingOutput ():
      #   return False

      # a bit unusual: check if input exist before scheduling the alg. Will nt report ERROR in input does not exist
      from RecExConfig.ObjKeyStore import objKeyStore
      if not objKeyStore.isInInput("ZdcDigitsCollection","ZdcDigitsCollection"):
         mlog.info("no ZdcDigitsCollection : Quit.")
         return False

      
      return True


    def configure(self):
        mlog = logging.getLogger ('ZdcRawChannelGetter.py::configure:')
        mlog.info('entering')

        from AthenaCommon.DetFlags import DetFlags
        if not DetFlags.detdescr.ZDC_on():
            mlog.info("not DetFlags.detdescr.Zdc_on : Quit.")
            return False

        if not DetFlags.makeRIO.ZDC_on():
            mlog.info("not DetFlags.makeRIO.Zdc_on : Quit.")
            return False


        try:
            from ZdcRec.ZdcRecConf import ZdcRec
            self._zdcRecHandle = ZdcRec()
        except Exception:
            mlog.error("could not get handle to ZdcRec")
            print traceback.format_exc()
            return False


        # output to ESD:
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addManyTypesStreamESD(self.output())
        # output to AOD:
        objKeyStore.addManyTypesStreamAOD(self.output())        

        # add to topsequence 
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.zdcRecHandle()

        return True

    def zdcRecHandle(self):
        return self._zdcRecHandle


    def outputKey(self) :
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def outputTypeKey(self) :
        return str(self.outputType()+"#"+self.outputKey())

