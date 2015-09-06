# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies Calo cell making
# so far only handle the RawChannel->CaloCell step
# not all possibility of CaloCellMaker_jobOptions.py integrated yet
from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import jobproperties as jp
from RecExConfig.Configured import Configured
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback

class CaloTowerEmGetter ( Configured )  :
    _outputType = "CaloTowerContainer"
    _output = { _outputType : "LArTowerEM" }

        
    def configure(self):
        mlog = logging.getLogger( 'CaloTowerEmGetter::configure :' )
        mlog.info ('entering')

        # get handle to upstream object
        # handle tile

        # handle LAr
        theCaloCellGetter = self.getInputGetter\
                            (jp.CaloRecFlags.clusterCellGetterName())
        

        # now configure the algorithm, part of this could be done in a separate class
        # cannot have same name
        try:        
            from CaloRec.CaloRecConf import CaloTowerAlgorithm                
        except:
            mlog.error("could not import CaloRec.CaloTowerAlgorithm")
            print traceback.format_exc()
            return False

        theCaloTowerAlgorithm=CaloTowerAlgorithm("EmTowerBldr")
        self._CaloTowerAlgorithmHandle = theCaloTowerAlgorithm ;


        # configure CaloTowerAlgorithm here
        


        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
            theLArTowerBuilderTool=LArTowerBuilderTool("LArEmTwrBldr")
        except:
            mlog.error("could not get handle to LArTowerBuilderTool or LArFCalTowerBuilderTool. Quit")
            print traceback.format_exc()
            return False

        theCaloTowerAlgorithm.TowerBuilderTools+= [ theLArTowerBuilderTool.getFullName() ]

            
        theCaloTowerAlgorithm.NumberOfPhiTowers=256
        theCaloTowerAlgorithm.NumberOfEtaTowers=200
        theCaloTowerAlgorithm.EtaMin=-2.5
        theCaloTowerAlgorithm.EtaMax=2.5

        theLArTowerBuilderTool.CellContainerName=theCaloCellGetter.outputKey()
        theLArTowerBuilderTool.IncludedCalos = [ "LAREM"]

        # add tool to alg . From now on theCaloClusterBuilderSW will point
        # on a COPY of the tool, so property cannot be further modified !
        theCaloTowerAlgorithm += theLArTowerBuilderTool

        # FIXME TowerSpy missing

        #

        # sets output key  
        theCaloTowerAlgorithm.TowerContainerName =self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        # LArTowerEM are not written in ESD
        # objKeyStore.addStreamESD(self.outputType(),self.outputKey())
        objKeyStore.addTransient(self.outputType(),self.outputKey())        


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        topSequence += theCaloTowerAlgorithm ;
        
        return True

    def caloTowerAlgorithmHandle(self):
        return self._CaloTowerAlgorithmHandle

   
# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType


