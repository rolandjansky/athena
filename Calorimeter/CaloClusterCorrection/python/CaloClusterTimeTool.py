# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def GetCaloClusterTimeTool(name, name_FebOffset='',name_RunOffset=''):

    from AthenaCommon.AppMgr import ToolSvc,theApp

  
    from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterTimeTool 
    
    theTool = CaloClusterTimeTool( name,
                                     keyFebOffset = name_FebOffset, 
                                     keyRunOffset = name_RunOffset 
                                 )  

    return theTool
