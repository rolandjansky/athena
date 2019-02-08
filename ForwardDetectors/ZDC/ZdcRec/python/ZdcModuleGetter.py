# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# specifies egamma"standard"
from AthenaCommon.Logging import logging

from RecExConfig.Configured import Configured


class ZdcModuleGetter ( Configured ) :
    _outputType = "xAOD::ZdcModuleContainer"

    _output = { _outputType: ["ZdcModules" ]}


    def preconfigure (self):
      mlog = logging.getLogger ('ZdcModuleGetter.py::preconfigure:')
      mlog.info('entering')

      preconfigureBase = super (ZdcModuleGetter,self).preconfigure() 

      
      if not preconfigureBase:
          return False

      #mlog = logging.getLogger( 'Configured::preconfigure:%s:' % self.__class__.__name__.replace( ".", '_' )  )
      #mlog.debug("Output= %s" % self.output() )
      #
      #if self.checkExistingOutput ():
      #   return False

      # a bit unusual: check if input exist before scheduling the alg. Will nt report ERROR in input does not exist
      from RecExConfig.ObjKeyStore import objKeyStore
      if not objKeyStore.isInInput("xAOD::TriggerTowerContainer","ZdcTriggerTowers"):
         mlog.info("no ZdcTriggerTowers: Quit.")
         return False

      
      return True


    def configure(self):
        mlog = logging.getLogger ('ZdcModuleGetter.py::configure:')
        mlog.info('entering')

        from AthenaCommon.DetFlags import DetFlags
        if not DetFlags.detdescr.ZDC_on():
            mlog.info("not DetFlags.detdescr.Zdc_on : Quit.")
            return False

        if not DetFlags.makeRIO.ZDC_on():
            mlog.info("not DetFlags.makeRIO.Zdc_on : Quit.")
            return False

        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon import CfgMgr
        #mlog.info("adding ZDC::ZdcAnalysisTool to ToolSvc with default parameters, and no calibrations enabled");
        #ToolSvc += CfgMgr.ZDC__ZdcAnalysisTool("ZdcAnalysisTool",DoCalib=False,Configuration="default")   
        zdcAnalysisTool = CfgMgr.ZDC__ZdcAnalysisTool("ZdcAnalysisTool",DoCalib=False,Configuration="pPb2016")   
        
        zdcAnalysisTool.FixTau1=True
        zdcAnalysisTool.FixTau2=True
        zdcAnalysisTool.Tau1=5
        zdcAnalysisTool.Tau2=21
        zdcAnalysisTool.Peak2ndDerivThresh=15

        try:
            from ZdcRec.ZdcRecConf import ZdcRecV3
            mlog.info("got ZdcRecV2")
            self._zdcRecHandle = ZdcRecV3(ZdcAnalysisTool = zdcAnalysisTool)
        except Exception:
            mlog.error("could not get handle to ZdcRecV3")
            import traceback
            traceback.print_exc()
            return False


        # output to ESD:
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addManyTypesStreamESD(self.output())

        # output to AOD:
        #W.L. 2016-04-15, removed for AOD-item list. See ATLASRECTS-3023 
        #objKeyStore.addManyTypesStreamAOD(self.output())        

        # add to topsequence 
        mlog.info("now adding to topSequence")
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.zdcRecHandle()
        mlog.info("added ZdcRecV3")

        return True

    def zdcRecHandle(self):
        return self._zdcRecHandle


    def outputKey(self) :
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def outputTypeKey(self) :
        return str(self.outputType()+"#"+self.outputKey())

