# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# EF Calo Calib Fex Algorithm Configuration:
# Ryan Mackenzie White <ryan.white@cern.ch> 
##############################

#from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFCaloCalibFex
from TrigEgammaHypo import TrigEgammaHypoConf
from AthenaCommon.SystemOfUnits import GeV, mm

from AthenaCommon.AppMgr import ToolSvc
from egammaRec.Factories import Factory, ToolFactory

from TrigEgammaRec.TrigEgammaToolFactories import TrigEgammaMVACalibTool
def configureTrigEFCaloCalibFexMonitoring(tool):
    
    from TrigEgammaHypo.TrigEFCaloHypoMonitoring import TrigEFCaloCalibFexValidationMonitoring, TrigEFCaloCalibFexOnlineMonitoring
    validation = TrigEFCaloCalibFexValidationMonitoring()
    online     = TrigEFCaloCalibFexOnlineMonitoring()
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig("Time")

    tool.AthenaMonTools = [ time, validation, online ]

TrigEFCaloCalibFex_Electron = Factory(TrigEgammaHypoConf.TrigEFCaloCalibFex, name = "TrigEFCaloCalibFex_Electron", doAdd=False, 
        AcceptAll = True,
        ApplyMVACalib = True,
        MVACalibTool = TrigEgammaMVACalibTool,
        egType = 'Electron',
        ClusterContainerKey = 'TrigEFCaloCalibFex',
        postInit = [configureTrigEFCaloCalibFexMonitoring],
        )

TrigEFCaloCalibFex_Photon = Factory(TrigEgammaHypoConf.TrigEFCaloCalibFex, name = "TrigEFCaloCalibFex_Photon", doAdd=False, 
        AcceptAll = True,
        ApplyMVACalib = True,
        MVACalibTool = TrigEgammaMVACalibTool,
        egType = 'Photon',
        ClusterContainerKey = 'TrigEFCaloCalibFex',
        postInit = [configureTrigEFCaloCalibFexMonitoring],
        )
