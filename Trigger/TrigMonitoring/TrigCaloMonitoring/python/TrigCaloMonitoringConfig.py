# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Pay attention this will return a list of tools
# in RAW->ESD and another for ESD->AOD
def HLTCaloMonitoringTool():

  if not 'DQMonFlags' in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags
  if DQMonFlags.monManEnvironment == 'tier0Raw':
    from TrigCaloRec.TrigCaloRecConf import RoILArEMCellContMaker, RoILArHadCellContMaker, RoITileCellContMaker, RoIFCalEmCellContMaker, RoIFCalHadCellContMaker, FullCaloCellContMaker

    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloTool,HLTCaloFEBTool
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool=CaloNoiseToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,"CaloNoiseToolDefault"):
      ToolSvc+=theCaloNoiseTool

    from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigDataAccess
    ToolSvc+=TrigDataAccess()
    ToolSvc.TrigDataAccess.loadAllSamplings =True
    
    roilaremcellcontmaker  = RoILArEMCellContMaker()
    roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
    roilarhadcellcontmaker = RoILArHadCellContMaker()
    roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
    roilaremcellcontmaker.DoLArCellsNoiseSuppression = 0
    roilarhadcellcontmaker.DoLArCellsNoiseSuppression = 0
    roifcalemcellcontmaker = RoIFCalEmCellContMaker()
    roifcalemcellcontmaker.CaloNoiseTool = theCaloNoiseTool
    roifcalemcellcontmaker.DoLArCellsNoiseSuppression = 0
    roifcalhadcellcontmaker = RoIFCalHadCellContMaker()
    roifcalhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
    roifcalhadcellcontmaker.DoLArCellsNoiseSuppression = 0
    #fullcalocellcontmaker = FullCaloCellContMaker()
    #fullcalocellcontmaker.CaloNoiseTool = theCaloNoiseTool
    #fullcalocellcontmaker.DoLArCellsNoiseSuppression = 0


    HLTCalo = HLTCaloTool(name             = 'HLTCalo',
                                        histoPathBase    = "/Trigger/HLT")
    HLTCalo.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roifcalemcellcontmaker,
                         roifcalhadcellcontmaker,
                         RoITileCellContMaker()]

  # From Ignacio
    HLTCaloFEB = HLTCaloFEBTool(name = 'HLTCaloFEB', 
                                        histoPathBase = "/Trigger/HLT")
    HLTCaloFEB.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roifcalemcellcontmaker,
                         roifcalhadcellcontmaker,
                         RoITileCellContMaker()]
    HLTCaloFEB.sigma = 3
#
    # Add Region Selector if there is none
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr,"RegSelSvcDefault"):
        from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
        ServiceMgr += RegSelSvcDefault()

    #HLTFullCalo = HLTCaloTool(name             = 'HLTFullCalo',
    #                                histoPathBase    = "/Trigger/HLT")
    #HLTFullCalo.TCRTools = [fullcalocellcontmaker]
    ToolSvc += HLTCalo;
    list = [ "HLTCaloTool/HLTCalo" ];
    ToolSvc += HLTCaloFEB;
    list += [ "HLTCaloFEBTool/HLTCaloFEB" ];
    #ToolSvc += HLTFullCalo;
    #list += [ "HLTCaloTool/HLTFullCalo" ];
    return list

  #elif DQMonFlags.monManEnvironment == 'tier0ESD':
  elif ( ( DQMonFlags.monManEnvironment == 'tier0ESD' ) or ( DQMonFlags.monManEnvironment == 'tier0' ) ) :
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloToolL2
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloClusterTool
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloESD_xAODTrigEMClusters
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloESD_xAODCaloClusters
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloESD_CaloCells

    xAODTrigEMClusters = HLTCaloESD_xAODTrigEMClusters("xAODTrigEMClusters")
    xAODCaloClusters   = HLTCaloESD_xAODCaloClusters  ("xAODCaloClusters")
    CaloCells          = HLTCaloESD_CaloCells         ("CaloCells")
        
    HLTCaloL2 = HLTCaloToolL2(name             = 'HLTCaloL2',
                                        histoPathBase    = "/Trigger/HLT",
                                        ListOfCellsKeys  =
                     ['HLT_CaloCellContainer_TrigT2CaloTauCells','HLT_CaloCellContainer_TrigT2CaloEgammaCells'])
    HLTCaloCluster =  HLTCaloClusterTool( name = 'HLTCaloCluster',
                                        histoPathBase    = "/Trigger/HLT")
    from AthenaCommon.AppMgr import ToolSvc
   #ToolSvc += HLTCaloL2;                                                                                                                                                                                                                                          
    ToolSvc += xAODTrigEMClusters
    #list = [ "HLTCaloToolL2/HLTCaloL2" ];                                                                                                                                                                                                                         
    list = [ "HLTCaloESD_xAODTrigEMClusters/xAODTrigEMClusters" ]
    #ToolSvc += HLTCaloCluster;                                                                                                                                                                                                                                    
    ToolSvc += xAODCaloClusters
    #list += [ "HLTCaloClusterTool/HLTCaloCluster" ];                                                                                                                                                                                                              
    list += [ "HLTCaloESD_xAODCaloClusters/xAODCaloClusters" ]
    ToolSvc += CaloCells
    list += [ "HLTCaloESD_CaloCells/CaloCells" ]
    
    return list
  else:
    list=[]
    return list

