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
        self._tools = {}
        self._orders = {}
        self.configure()

    def configure(self):

        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'Configured::__init__:%s:' % self.__class__.__name__.replace( ".", '_' )  )
        mlog.info('InputClusterKeys %s ', self.inputClusterKeys())
        mlog.info('InputCellKey %s '    , self.inputCellKey())
        mlog.info('OutputCellKey %s '   , self.outputCellKey())

        from AthenaCommon.AppMgr import ToolSvc

        #Schedule the calo cell maker. 
        from CaloRec.CaloRecConf import CaloConstCellMaker
        #======================
        self.theCaloCellMaker=CaloConstCellMaker(name="CaloCellMakerFromCluster_"+self.outputCellKey(),
                                                 CaloCellsOutputName=self.outputCellKey(),
                                                 OwnPolicy=1)

        self._CaloCellMakerHandle = self.theCaloCellMaker
        #Add the needed tool instances, only if they do not exist already

        #====================== CaloCellContainerFromClusterTool
        if not hasattr(ToolSvc,'CaloCellContainerFromClusterTool_'+self.outputCellKey()):
            self.theCaloCellContainerFromClusterTool = self.GetTool('CaloCellContainerFromClusterTool_'+self.outputCellKey())
            if self.theCaloCellContainerFromClusterTool is None:
                from CaloRec.CaloRecConf import CaloCellContainerFromClusterTool
                self.theCaloCellContainerFromClusterTool = CaloCellContainerFromClusterTool('CaloCellContainerFromClusterTool_'+self.outputCellKey())
                ToolSvc += self.theCaloCellContainerFromClusterTool
                self.AddTool(self.theCaloCellContainerFromClusterTool, order = 1)
                self.theCaloCellContainerFromClusterTool.CaloCellName=self.inputCellKey()
                self.theCaloCellContainerFromClusterTool.AddSamplingCells = True
                self.theCaloCellContainerFromClusterTool.SamplingCellsName = ["TileGap1", "TileGap2", "TileGap3", "TileBar0","TileExt0", "HEC0"]
                mlog.info('CaloCellContainerFromClusterTool has been added')
        else:
            self.theCaloCellContainerFromClusterTool = getattr(ToolSvc,'CaloCellContainerFromClusterTool_'+self.outputCellKey())

        #====================== CaloCellFastCopyTool
        if not hasattr(ToolSvc,'CaloCellFastCopyTool_'+self.outputCellKey()):
            self.theCaloCellFastCopyTool = self.GetTool('CaloCellFastCopyTool_'+self.outputCellKey())
            if self.theCaloCellFastCopyTool is None:
                from CaloRec.CaloRecConf import CaloCellFastCopyTool
                self.theCaloCellFastCopyTool = CaloCellFastCopyTool('CaloCellFastCopyTool_'+self.outputCellKey())
                self.theCaloCellFastCopyTool.InputName=self.inputCellKey()
                self.theCaloCellFastCopyTool.AvoidDuplicates = True
                self.theCaloCellFastCopyTool.IsFindCellFast = True
                self.theCaloCellFastCopyTool.IncludeSamplings += [ "TileGap3" ]
                mlog.info('The sampling %s has been scheduled to be copied ', "TileGap3")
                ToolSvc += self.theCaloCellFastCopyTool
                self.AddTool(self.theCaloCellFastCopyTool, order = 7)
                mlog.info('CaloCellFastCopyTool has been added')
        else:
            self.theCaloCellFastCopyTool = getattr(ToolSvc,'CaloCellFastCopyTool_'+self.outputCellKey())
        
        #======================CaloCellContainerFinalizerTool
        if not hasattr(ToolSvc,'CaloCellContainerFinalizerTool_'+self.outputCellKey()):
            self.theCaloCellContainerFinalizerTool = self.GetTool('CaloCellContainerFinalizerTool_'+self.outputCellKey())
            if self.theCaloCellContainerFinalizerTool is None:
                from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
                self.theCaloCellContainerFinalizerTool=CaloCellContainerFinalizerTool('CaloCellContainerFinalizerTool_'+self.outputCellKey())
                ToolSvc+=self.theCaloCellContainerFinalizerTool
                self.AddTool(self.theCaloCellContainerFinalizerTool, order = 9)
                mlog.info('CaloCellContainerFinalizerTool has been added')
        else:
            self.theCaloCellContainerFinalizerTool = getattr(ToolSvc,'CaloCellContainerFinalizerTool_'+self.outputCellKey())
        
        #====================== CaloCellContainerCheckerTool
        if not hasattr(ToolSvc,'CaloCellContainerCheckerTool_'+self.outputCellKey()):
            self.theCaloCellContainerCheckerTool = self.GetTool('CaloCellContainerCheckerTool_'+self.outputCellKey())
            if self.theCaloCellContainerCheckerTool  is None:
                from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
                self.theCaloCellContainerCheckerTool=CaloCellContainerCheckerTool('CaloCellContainerCheckerTool_'+self.outputCellKey())
                ToolSvc+=self.theCaloCellContainerCheckerTool
                self.AddTool(self.theCaloCellContainerCheckerTool, order = 10)
                mlog.info('CaloCellContainerCheckerTool has been added')
        else:
            self.theCaloCellContainerCheckerTool = getattr(ToolSvc,'CaloCellContainerCheckerTool_'+self.outputCellKey())
        
        #======================
        #Algorithm to update cluster->cell links
        from CaloRec.CaloRecConf import CaloClusterCellLinksUpdater
        self.theClusterUpdater=CaloClusterCellLinksUpdater(name="CaloClusterCellLinksUpdater_"+self.outputCellKey(),
                                                           NewCaloCellName=self.outputCellKey())   
        self._ClusterUpdateHandle=self.theClusterUpdater
 
        for inputname in self.inputClusterKeys():    
            self.theCaloCellContainerFromClusterTool.CaloClusterNames+=[inputname]
            self.AddClusterToUpdate(inputname)
            mlog.info("Add Clusters Collection  %s to the %s CellCollection ",inputname,self.outputCellKey())
        
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

    def CaloCellMakerHandle(self):
        return self._CaloCellMakerHandle
 
    def outputCellKey(self):
        return self._outputCellKey    

    def inputClusterKeys(self):
        return self._inputClusterKeys

    def inputCellKey(self):
        return self._inputCellKey

    def outputType(self):
        return self._outputType
    
    def AddCluster(self,name):
        self.theCaloCellContainerFromClusterTool.CaloClusterNames+=[name]
        self.AddClusterToUpdate(name)
        mlog.info("Add Cluster Collection  %s to the %s CellCollection ",name,self.outputCellKey())

    def GetTool(self, toolName):
        if toolName in self._tools:
            return self._tools[toolName]
        return None
    
    def AddClusterToUpdate(self,clusterkey):
        self._ClusterUpdateHandle.CaloClusterNames+=[clusterkey,]
        return None

    def AddTool(self, tool, order):
        if tool.getName() in self._tools:
            return False
        self._CaloCellMakerHandle.CaloCellMakerTools+= [tool]
        self._tools[tool.getName()] = tool
        self._orders[tool.getName()] = order 
        self._CaloCellMakerHandle.CaloCellMakerTools.sort(key = lambda t: self._orders[t.getName()])
        return True
         
#==========================================================================================

