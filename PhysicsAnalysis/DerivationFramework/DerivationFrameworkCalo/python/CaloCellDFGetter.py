# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#==========================================================================================
#This is basically based in the  Calorimeter/CaloRec/python/CaloCellAODGetter.py (used for ESD to AOD)
#It is  modified as  to be usable in derivation.
#
# The understanding is that in derivation we will run multiple "instances"
# of this algorithm for the different derivations. These could be scheduled in a "train"
# one after the other in the same AlgSequence. This is unlike ESD->AOD when we 
# need only one instance.
#
# Usage : 
#
#from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
#theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters"],
#                                       outputCellKey="DFEGAM1CellContainer")
#
#Using a custom sequence
#
#DESDM_EGAMMA_Seq = CfgMgr.AthSequencer("DESDM_EGAMMA_Seq")
#from DerivationFrameworkCalo.CaloCellDFGetter import CaloCellDFGetter
#theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters"],
#                                       outputCellKey="DFEGAM1CellContainer",
#                                       Sequense=DESDM_EGAMMA_Seq)
#
#
# One can add more cluster cells in a different container from a different derivation
#
#
#theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["MuonClusterCollection"],
#                                       outputCellKey="DFMUON1CellContainer")
#
# Or many in the same container from another derivation
#
#theCaloCellDFGetter = CaloCellDFGetter(inputClusterKeys=["egammaClusters","MuonClusterCollection"],
#                                       outputCellKey="DFHIGG1CellContainer")
#
# Do not forget to write out the cell collection and the _links
#
#EGAM1Stream.AddItem("CaloClusterCellLinkContainer#egammaClusters_links")
#EGAM1Stream.AddItem("CaloCellContainer#DFEGAM1CellContainer")

# Also in principle everything has to belong to a cluster. 
# For example in ESD to AOD we create 7x11 cluster
# to write extra cell for e/gamma
#==========================================================================================

from AthenaCommon.Constants import *  
from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()

class CaloCellDFGetter () :
   
    def __init__(self,inputClusterKeys=["egammaClusters"], 
                 inputCellKey="AODCellContainer", 
                 outputCellKey="DFCellContainer",
                 Sequense=topSequence):

        self._outputCellKey=outputCellKey
        self._inputClusterKeys=inputClusterKeys
        self._inputCellKey=inputCellKey
        self._outputType= "CaloCellContainer"
        self._sequence=Sequense
        self.configure()

    def configure(self):

        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'Configured::__init__:%s:' % self.__class__.__name__.replace( ".", '_' )  )
        mlog.info('InputClusterKeys %s ', self.inputClusterKeys())
        mlog.info('InputCellKey %s '    , self.inputCellKey())
        mlog.info('OutputCellKey %s '   , self.outputCellKey())


        #Schedule the calo cell maker. 
        from CaloRec.CaloRecConf import CaloConstCellMaker
        #======================
        self.theCaloCellMaker=CaloConstCellMaker(name="CaloCellMakerFromCluster_"+self.outputCellKey(),
                                                 CaloCellsOutputName=self.outputCellKey(),
                                                 OwnPolicy=1)

        self._CaloCellMakerHandle = self.theCaloCellMaker
        #Add the needed tool instances, only if they do not exist already

        #====================== CaloCellContainerFromClusterTool
        from CaloRec.CaloRecConf import CaloCellContainerFromClusterTool
        self.theCaloCellMaker.CaloCellMakerTools+=[CaloCellContainerFromClusterTool('CaloCellContainerFromClusterTool_'+self.outputCellKey(),
                                                                             CaloCellName=self.inputCellKey(),
                                                                             AddSamplingCells = True,
                                                                             SamplingCellsName = ["TileGap1", "TileGap2", "TileGap3", "TileBar0","TileExt0", "HEC0"],
                                                                         ),]
        #====================== CaloCellFastCopyTool
        from CaloRec.CaloRecConf import CaloCellFastCopyTool
        self.theCaloCellMaker.CaloCellMakerTools+=[CaloCellFastCopyTool('CaloCellFastCopyTool_'+self.outputCellKey(),
                                                                  InputName=self.inputCellKey(),
                                                                  AvoidDuplicates = True,
                                                                  IsFindCellFast = True,
                                                                  IncludeSamplings = [ "TileGap3", ]
                                                              ),]
        
        #======================CaloCellContainerFinalizerTool
        from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
        self.theCaloCellMaker.CaloCellMakerTools+=[CaloCellContainerFinalizerTool('CaloCellContainerFinalizerTool_'+self.outputCellKey()),]
        
                
        #====================== CaloCellContainerCheckerTool
        from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
        self.theCaloCellMaker.CaloCellMakerTools+=[CaloCellContainerCheckerTool('CaloCellContainerCheckerTool_'+self.outputCellKey()),]

        #Algorithm to update cluster->cell links
        from CaloRec.CaloRecConf import CaloClusterCellLinksUpdater
        self.theClusterUpdater=CaloClusterCellLinksUpdater(name="CaloClusterCellLinksUpdater_"+self.outputCellKey(),
                                                           NewCaloCellName=self.outputCellKey())   
        self._ClusterUpdateHandle=self.theClusterUpdater
 
        for inputname in self.inputClusterKeys():    
            self.AddCluster(inputname)
        
        #======================
        #Avoid scheduling the same algorithm instances twice

        if  not hasattr(self._sequence,"CaloCellMakerFromCluster_"+self.outputCellKey()):
            mlog.info("Make Cell Collection  %s ",  self.outputCellKey())
            self._sequence+=self.theCaloCellMaker 
        else :
            mlog.info("CaloCellMakerFromCluster_%s exists already not adding it again",self.outputCellKey())        
        if not hasattr(self._sequence,"CaloClusterCellLinksUpdater_"+self.outputCellKey()):        
            mlog.info("Update _links for %s to point at %s", str(self.inputClusterKeys()), self.outputCellKey())
            self._sequence +=self.theClusterUpdater
        else :
            mlog.info("CaloClusterCellLinksUpdater_%s exists already not adding it again",self.outputCellKey())    

        mlog.info('CaloCellMakerHandle list of Tools:')
        mlog.info(self._CaloCellMakerHandle.CaloCellMakerTools)    

        return True

    def outputCellKey(self):
        return self._outputCellKey    

    def inputClusterKeys(self):
        return self._inputClusterKeys

    def inputCellKey(self):
        return self._inputCellKey

    def outputType(self):
        return self._outputType
    
    def AddCluster(self,name):
        self._CaloCellMakerHandle.CaloCellMakerTools[0].CaloClusterNames+=[name,]
        self._ClusterUpdateHandle.CaloClusterNames+=[name,]
        #mlog.info("Add Cluster Collection  %s to the %s CellCollection ",name,self.outputCellKey())

#==========================================================================================

