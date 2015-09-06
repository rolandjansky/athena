# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies CaloCell-> CaloTower combined 
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback

from RecExConfig.Configured import Configured

class CaloClusterSWCmbGetter ( Configured )  :
    _outputType = "CaloClusterContainer"
    _outputKey = "CombinedCluster"
    _output = { _outputType : _outputKey , 
                "CaloClusterAuxContainer" : _outputKey+"Aux.",
                "CaloCellLinkContainer" : _outputKey+"_links" 
                } # exact list of output

        
    def configure(self):
        mlog = logging.getLogger( 'CaloClusterSWCmb.py::configure :' )
        mlog.info ('entering')

        # get handle to upstream object
        # combined calo tower
        try:
            from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
            theCaloTowerCmbGetter=CaloTowerCmbGetter()
        except:
            mlog.error("could not get handle to CaloTowerCmbGetter Quit")
            print traceback.format_exc()
            return False

            


        if not theCaloTowerCmbGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("CaloTowerCmbGetter unusable. Quit.")
                return False
            else:
                mlog.error("CaloTowerCmbGetter unusable. Continue nevertheless")
                
        

        # now configure the algorithm, part of this could be done in a separate class
        # cannot have same name
        try:        
            from CaloRec.CaloRecConf import CaloClusterMaker                
        except:
            mlog.error("could not import CaloRec.CaloClusterMaker")
            print traceback.format_exc()
            return False

        theCaloClusterMaker=CaloClusterMaker("CaloClusterMakerSWCmb")
        self._CaloClusterMakerHandle = theCaloClusterMaker ;


        # configure CaloClusterMaker here
        

        try:
            from CaloRec.CaloRecConf import CaloClusterBuilderSW
            theCaloClusterBuilderSW=CaloClusterBuilderSW("CaloClusterBuilderSWCmb")
        except:
            mlog.error("could not get handle to CaloClusterBuilderSWCmb Quit")
            print traceback.format_exc()
            return False

        # add the tool to list of tool ( should use ToolHandle eventually) 
        theCaloClusterMaker.ClusterMakerTools+= [ theCaloClusterBuilderSW.getFullName() ]

        theCaloClusterBuilderSW.TowerContainer=theCaloTowerCmbGetter.outputKey()
        theCaloClusterBuilderSW.eta_size=5
        theCaloClusterBuilderSW.phi_size=5
        theCaloClusterBuilderSW.eta_sizep=3
        theCaloClusterBuilderSW.phi_sizep=3
        theCaloClusterBuilderSW.e_threshold=15.*GeV
        theCaloClusterBuilderSW.FillClusterCells=True #fill cells in ClusterBuilderSW
        theCaloClusterBuilderSW.nextra=0
        theCaloClusterBuilderSW.eta_SeedGrid=5
        theCaloClusterBuilderSW.phi_SeedGrid=5
        theCaloClusterBuilderSW.eta_Duplicate=5
        theCaloClusterBuilderSW.phi_Duplicate=5


        # add tool to alg . From now on theCaloClusterBuilderSW will point
        # on a COPY of the tool, so property cannot be further modified !
        theCaloClusterMaker+=theCaloClusterBuilderSW


        # sets output key  
        theCaloClusterMaker.ClustersOutputName =self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore


        # write everything in ESD
        objKeyStore.addManyTypesStreamESD(self.output())
        # only write cluster class in AOD
        objKeyStore.addStreamAOD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        topSequence += theCaloClusterMaker ;
        
        return True

    def caloClusterMakerHandle(self):
        return self._CaloClusterMaker

    @classmethod
    def outputKey(cls):
        return cls._output[cls._outputType]

    @classmethod
    def outputType(cls):
        return cls._outputType



