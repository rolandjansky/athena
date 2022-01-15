# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# specifies CaloCell-> CaloTower combined 
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import MeV
import traceback

from RecExConfig.Configured import Configured

class CaloTowerCmbGetter ( Configured )  :
    _outputType = "CaloTowerContainer"
    _output = { _outputType : "CombinedTower" }

        
    def configure(self):
        mlog = logging.getLogger( 'CaloTowerCmbGetter::configure :' )
        mlog.info ('scheduled to output %s',self.output())


        # get handle to upstream object
        # handle tile

        # handle LAr
        #theCaloCellGetter = self.getInputGetter\
        #                    (jp.CaloRecFlags.clusterCellGetterName())
                

        # now configure the algorithm, part of this could be done in a separate class
        # cannot have same name
        try:        
            from CaloRec.CaloRecConf import CaloTowerAlgorithm                
            theCaloTowerAlgorithm=CaloTowerAlgorithm("CmbTowerBldr")
        except Exception:
            mlog.error("could not import CaloRec.CaloTowerAlgorithm")
            print(traceback.format_exc())
            return False


        self._CaloTowerAlgorithmHandle = theCaloTowerAlgorithm


        # configure CaloTowerAlgorithm here
        

        try:
            from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
            theTileTowerBuilderTool=TileTowerBuilderTool("TileCmbTwrBldr")
        except Exception:
            mlog.error("could not get handle to TileTowerBuilderTool Quit")
            print(traceback.format_exc())
            return False

        # add the tool to list of tool ( should use ToolHandle eventually) 
        theCaloTowerAlgorithm.TowerBuilderTools+= [ theTileTowerBuilderTool.getFullName() ]

        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool,LArFCalTowerBuilderTool
            theLArTowerBuilderTool=LArTowerBuilderTool("LArCmbTwrBldr")
            theLArFCalTowerBuilderTool=LArFCalTowerBuilderTool("LArFCalCmbTwrBldr")            
        except Exception:
            mlog.error("could not get handle to LArTowerBuilderTool or LArFCalTowerBuilderTool. Quit")
            print(traceback.format_exc())
            return False

        theCaloTowerAlgorithm.TowerBuilderTools+= [ theLArTowerBuilderTool.getFullName() ]
        theCaloTowerAlgorithm.TowerBuilderTools+= [ theLArFCalTowerBuilderTool.getFullName() ]
            
        theCaloTowerAlgorithm.NumberOfPhiTowers=64
        theCaloTowerAlgorithm.NumberOfEtaTowers=100
        theCaloTowerAlgorithm.EtaMin=-5.0
        theCaloTowerAlgorithm.EtaMax=5.0

        theLArTowerBuilderTool.CellContainerName="AllCalo"
        theLArTowerBuilderTool.IncludedCalos = [ "LAREM","LARHEC" ]

        theLArFCalTowerBuilderTool.CellContainerName="AllCalo"
        theLArFCalTowerBuilderTool.MinimumEt = 0 * MeV
        
        theTileTowerBuilderTool.CellContainerName="AllCalo"


        # add tool to alg . From now on theCaloClusterBuilderSW will point
        # on a COPY of the tool, so property cannot be further modified !
        theCaloTowerAlgorithm += theLArTowerBuilderTool
        theCaloTowerAlgorithm += theLArFCalTowerBuilderTool        
        theCaloTowerAlgorithm += theTileTowerBuilderTool

        # FIXME TowerSpy missing

        #

        # sets output key  
        theCaloTowerAlgorithm.TowerContainerName =self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        topSequence += theCaloTowerAlgorithm
        
        return True

    def caloTowerAlgorithmHandle(self):
        return self._CaloTowerAlgorithmHandle

   
# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType


