# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# jobOption to create VIEW CaloCellContainer based on one (or more) 
# clusters or other tools. 


from AthenaCommon.Constants import *  # DEBUG in there
from RecExConfig.Configured import Configured
class CaloCellAODGetter (Configured) :
    _outputType= "CaloCellContainer"
    _output = { _outputType : "AODCellContainer" }
    
    def configure(self):
        
        from CaloRec.CaloRecConf import CaloConstCellMaker
        theCaloCellMaker=CaloConstCellMaker("CaloCellMakerFromCluster",
                    CaloCellsOutputName=self.outputKey(),
                    OwnPolicy=1)
        self._CaloCellMakerHandle = theCaloCellMaker
        
        from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
        theCaloCellContainerFinalizerTool=CaloCellContainerFinalizerTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloCellContainerFinalizerTool
        theCaloCellMaker.CaloCellMakerTools+=[theCaloCellContainerFinalizerTool]


        from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
        theCaloCellContainerCheckerTool=CaloCellContainerCheckerTool()
        ToolSvc+=theCaloCellContainerCheckerTool
        theCaloCellMaker.CaloCellMakerTools+=[theCaloCellContainerCheckerTool]


        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        topSequence += theCaloCellMaker 

        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addStreamAOD(self.outputType(),self.outputKey())

        #Schedule algorithm to update cluster->cell links
        from CaloRec.CaloRecConf import CaloClusterCellLinksUpdater
        theClusterUpdater=CaloClusterCellLinksUpdater(NewCaloCellName=self._output["CaloCellContainer"])   
        self._ClusterUpdateHandle=theClusterUpdater
        topSequence+=theClusterUpdater
        
        return True


    def CaloCellMakerHandle(self):
        return self._CaloCellMakerHandle

    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

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
        self._CaloCellMakerHandle.CaloCellMakerTools += [tool]
        self._tools[tool.getName()] = tool
        self._orders[tool.getName()] = order

        self._CaloCellMakerHandle.CaloCellMakerTools.sort(key = lambda t: self._orders[t.getName()])
        return True
        
    _tools = {}
    _orders = {'CaloCellContainerFinalizerTool':   9,   \
               'CaloCellContainerCheckerTool':    10}





# This can be used to write a sparse cell container into the AOD, provided
# that this jO fragment is called between the ESD and AOD writing, the
# ElementLinks of the cluster are updated and the corresponding CellLink 
# Container is written to the AOD as well. 
#
# This function to use to add cell of a given cluster in AOD
# usage:
# from CaloRec.CaloCellAODGetter import addClusterToCaloCellAOD
# addClusterToCaloCellAOD("MuonClusterCollection")

def addClusterToCaloCellAOD(clustersInputName):
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'addClusterToCaloCellAOD' )

    from RecExConfig.ObjKeyStore import objKeyStore

## comment out until objKeyStore is automatically filled from input content
##     if not objKeyStore.isInInput("CaloClusterContainer",clustersInputName):
##         mlog.warning ("CaloClusterContainer %s does not exist. Not triggering it " % clustersInputName )
##         return
        
    try:
        from CaloRec.CaloCellAODGetter import CaloCellAODGetter
        theCaloCellAODGetter = CaloCellAODGetter()
        if not theCaloCellAODGetter.usable():
            mlog.warning ("CaloCellAODGetter not usable ")
            return

        theCaloCellContainerFromClusterTool = theCaloCellAODGetter.GetTool('CaloCellContainerFromClusterTool')
        if theCaloCellContainerFromClusterTool is None:
            from CaloRec.CaloRecConf import CaloCellContainerFromClusterTool
            theCaloCellContainerFromClusterTool = CaloCellContainerFromClusterTool()

            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += theCaloCellContainerFromClusterTool
            theCaloCellAODGetter.AddTool(theCaloCellContainerFromClusterTool, order = 1)
            theCaloCellContainerFromClusterTool.AddSamplingCells = True
            theCaloCellContainerFromClusterTool.SamplingCellsName = ["TileGap1", "TileGap2", "TileGap3", "TileBar0","TileExt0", "HEC0"]
            mlog.info('CaloCellContainerFromClusterTool has been added to CaloCellAODGetter')

        theCaloCellContainerFromClusterTool.CaloClusterNames += [clustersInputName]
        theCaloCellAODGetter.AddClusterToUpdate(clustersInputName)

        addCaloSamplingToCaloCellAOD("TileGap3")

    except Exception:
        mlog.warning ("Could not configure CaloCellAODGetter ")
        import traceback
        print traceback.format_exc() 

# This can be used to write a sparse cell container into the AOD
# with additional specified CaloSampling, provided
# that this jO fragment is called between the ESD and AOD writing, 
# Container is written to the AOD as well. 
def addCaloSamplingToCaloCellAOD(samplingName):
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger('addCaloSamplingToCaloCellAOD')

    try:
        from CaloRec.CaloCellAODGetter import CaloCellAODGetter
        theCaloCellAODGetter = CaloCellAODGetter()
        if not theCaloCellAODGetter.usable():
            mlog.warning ("CaloCellAODGetter not usable ")
            return

        theCaloCellFastCopyTool = theCaloCellAODGetter.GetTool('CaloCellFastCopyTool')
        if theCaloCellFastCopyTool is None:
            from AthenaCommon.AppMgr import ToolSvc
            from CaloRec.CaloRecConf import CaloCellFastCopyTool
            theCaloCellFastCopyTool = CaloCellFastCopyTool()
            ToolSvc += theCaloCellFastCopyTool
            theCaloCellAODGetter.AddTool(theCaloCellFastCopyTool, order = 7)
            mlog.info('CaloCellFastCopyTool has been added to CaloCellAODGetter')
        
        theCaloCellFastCopyTool.AvoidDuplicates = True
        theCaloCellFastCopyTool.IsFindCellFast = True
        theCaloCellFastCopyTool.IncludeSamplings += [ samplingName ]

        mlog.info('The sampling has been scheduled to be copied to AODCellContainer: ' + samplingName)

    except Exception:
        mlog.warning ("Could not configure CaloCellAODGetter ")
        import traceback
        print traceback.format_exc() 
