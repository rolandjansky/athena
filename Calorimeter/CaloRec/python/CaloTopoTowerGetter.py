# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# specifies Tower/TopoCluster -> TopoTower
from AthenaCommon.JobProperties import jobproperties as jp
from AthenaCommon.Logging import logging
import traceback

from RecExConfig.Configured import Configured

class CaloTopoTowerGetter ( Configured )  :
        _outputType = "CaloTopoTowerContainer"
        _output = { _outputType : "TopoTower" }
        

        def configure(self):
                mlog = logging.getLogger( 'CaloTopoTowerGetter::configure :' )
                mlog.info ('scheduled to output %s',self.output())

                # get handle to upstream CaloCells
                theCaloCellGetter = self.getInputGetter\
                                    (jp.CaloRecFlags.clusterCellGetterName())

                # ----- get handle to upstream tower objects
                try:
                    from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
                    theCaloTowerCmbGetter = CaloTowerCmbGetter()
                except Exception:
                    mlog.error("could not get handle to CaloTowerCmbGetter Quit")
                    print(traceback.format_exc())
                    return False

                if not theCaloTowerCmbGetter.usable():
                    if not self.ignoreConfigError():
                        mlog.error("CaloTowerCmbGetter unusable. Quit.")
                        return False
                    else:
                        mlog.error("CaloTowerCmbGetter unusable. Continue nevertheless")


                # ------ get handle to upstream topo cluster object
                try:
                    from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter
                    theCaloClusterTopoGetter = CaloClusterTopoGetter()
                except Exception:
                    mlog.error("could not get handle to CaloClusterTopoGetter  Quit")
                    print(traceback.format_exc())
                    return False
        
                if not theCaloClusterTopoGetter.usable():
                    if not self.ignoreConfigError():
                        mlog.error("theCaloClusterTopoGetter unusable. Quit.")
                        return False
                    else:
                        mlog.error("theCaloClusterTopoGetter unusable. Continue nevertheless")

                # ------ get handle to upstream topo cluster object
                try:
                    from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter
                    CaloClusterTopoGetter()
                    from CaloRec.CaloCell2ClusterMapperGetters import CaloCell2TopoClusterMapperGetter
                    theCaloCell2TopoClusterMapperGetter = CaloCell2TopoClusterMapperGetter()
                except Exception:
                    mlog.error("could not get handle to CaloCell2TopoClusterMapperGetter  Quit")
                    print(traceback.format_exc())
                    return False

                if not theCaloCell2TopoClusterMapperGetter.usable():
                    if not self.ignoreConfigError():
                        mlog.error("theCaloCell2TopoClusterMapperGetter unusable. Quit.")
                        return False
                    else:
                        mlog.error("theCaloCell2TopoClusterMapperGetter unusable. Continue nevertheless")

                # now configure the algorithm, part of this could be done in a separate class
                # cannot have same name
                try:        
                    from CaloRec.CaloRecConf import CaloTopoTowerAlgorithm                
                    theCaloTopoTowerAlgorithm=CaloTopoTowerAlgorithm("TopoTowerBldr")
                except Exception:
                    mlog.error("could not import CaloRec.CaloTopoTowerAlgorithm")
                    print(traceback.format_exc())
                    return False

                self._CaloTopoTowerAlgorithmHandle = theCaloTopoTowerAlgorithm


                # configure CaloTopoTowerAlgorithm here
                try:
                    from CaloUtils.CaloUtilsConf import CaloTopoTowerBuilderTool
                    theCaloTopoTowerBuilderTool=CaloTopoTowerBuilderTool("TopoTowerTwrBldr")
                except Exception:
                    mlog.error("could not get handle to CaloTopoTowerBuilderTool Quit")
                    print(traceback.format_exc())
                    return False
    
                theCaloTopoTowerAlgorithm.TowerBuilderTools+= [ theCaloTopoTowerBuilderTool.getFullName() ]

        
                ########################
                # extra cuts which can be applied at the topo tower level
                # set input keys
                theCaloTopoTowerAlgorithm.InputTowerContainerName = theCaloTowerCmbGetter.outputKey()
                theCaloTopoTowerAlgorithm.ClusterContainerName = theCaloClusterTopoGetter.outputKey()
                theCaloTopoTowerAlgorithm.CellContainerName =  theCaloCellGetter.outputKey()
                theCaloTopoTowerAlgorithm.Cell2ClusterMapName = theCaloCell2TopoClusterMapperGetter.outputKey()

                # sets output key
                theCaloTopoTowerAlgorithm.OutputTowerContainerName =self.outputKey()

                theCaloTopoTowerAlgorithm.MinimumCellEnergy      = -1000000000.0
                theCaloTopoTowerAlgorithm.MinimumClusterEnergy   = -1000000000.0
                theCaloTopoTowerAlgorithm.CellEnergySignificance = -1.


                ########################

                # add tool to alg . From now on theCaloClusterBuilderSW will point
                # on a COPY of the tool, so property cannot be further modified !

                theCaloTopoTowerAlgorithm += theCaloTopoTowerBuilderTool

                # register output in objKeyStore
                from RecExConfig.ObjKeyStore import objKeyStore
                objKeyStore.addStreamESD(self.outputType(),self.outputKey())

                # now add algorithm to topSequence
                # this should always come at the end

                mlog.info(" now adding to topSequence")        

                from AthenaCommon.AlgSequence import AlgSequence
                topSequence = AlgSequence()

                topSequence += theCaloTopoTowerAlgorithm

                return True

        def caloTowerAlgorithmHandle(self):
                return self._CaloTopoTowerAlgorithmHandle


        # would work only if one output object type
        def outputKey(self):
                return self._output[self._outputType]

        def outputType(self):
                return self._outputType
