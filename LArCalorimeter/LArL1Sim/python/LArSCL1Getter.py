# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# LArSCL1 creation from LArHits with LArSCL1Maker algorithm

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
import traceback

class LArSCL1Getter ( Configured )  :

        
    def configure(self):
        mlog = logging.getLogger( 'LArSCL1Getter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from LArL1Sim.LArSCL1Getter import LArSCL1Getter
            theLArSCL1Getter=LArSCL1Getter()
        except Exception:
            mlog.error("could not get handle to LArSCL1Getter Quit")
            traceback.print_exc()
            return False

        if not theLArSCL1Getter.usable():
            if not self.ignoreConfigError():
                mlog.error("LArSCL1Getter unusable. Quit.")
                return False
            else:
                mlog.error("LArSCL1Getter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from LArL1Sim.LArL1SimConf import LArSCL1Maker                
        except Exception:
            mlog.error("could not import LArL1Sim.LArSCL1Maker")
            traceback.print_exc()
            return False

        theLArSCL1Maker=LArSCL1Maker()
        from AthenaCommon.AppMgr import ToolSvc
        from LArRecUtils.LArAutoCorrNoiseSCToolDefault import LArAutoCorrNoiseSCToolDefault
        theLArAutoCorrNoiseSCTool = LArAutoCorrNoiseSCToolDefault()
        ToolSvc+=theLArAutoCorrNoiseSCTool
        theLArSCL1Maker.AutoCorrNoiseTool = theLArAutoCorrNoiseSCTool

        from LArRecUtils.LArADC2MeVSCToolDefault import LArADC2MeVSCToolDefault
        theLArADC2MeVSCTool = LArADC2MeVSCToolDefault()
        ToolSvc+=theLArADC2MeVSCTool
        theLArSCL1Maker.ADC2MeVTool = theLArADC2MeVSCTool

        theLArSCL1Maker.SCL1ContainerName = "LArDigitSCL1"

        self._LArSCL1Maker = theLArSCL1Maker

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        # check if LArdigitization is run before. If yes, uses hit map from detector store produces from lardigitization
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.digitize.LAr_on():
            mlog.info("Using hit map from LArDigitMaker algoritm")
        else:
            mlog.info("digitmaker1 not found in topSequence, using own map in LArSCL1Maker")
            return False
        
        # now add algorithm to topSequence
        # this should always come at the end
        from IOVDbSvc.CondDB import conddb
        if ( conddb.isMC and not conddb.folderRequested('/LAR/IdentifierOfl/OnOffIdMap_SC') ) :
            conddb.addFolder("LAR_OFL","<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>/LAR/IdentifierOfl/OnOffIdMap_SC")
        if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/fSampl') ) :
            conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCfSampl-000</tag>/LAR/ElecCalibMCSC/fSampl")
        if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Pedestal') ) :
            conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCPedestal-000</tag>/LAR/ElecCalibMCSC/Pedestal")
        if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Noise') ) :
            conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCNoise-000</tag>/LAR/ElecCalibMCSC/Noise")
        if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Shape') ) :
            conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCShape-000</tag>/LAR/ElecCalibMCSC/Shape")

        from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr,"LArFlatConditionSvc"):
           svcMgr+=LArFlatConditionSvc()
           svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
        svcMgr.LArFlatConditionSvc.DoSuperCells=True

        mlog.info(" now adding to topSequence")        
        topSequence += theLArSCL1Maker

        
        return True

    def LArSCL1Maker(self):
        return self._LArSCL1Maker
   
    def outputKey(cls):
       return cls._output[cls._outputType]

    def outputType(cls):
       return cls._outputType

    def outputTypeKey(self):
       return str(self.outputType()+"#"+self.outputKey())


