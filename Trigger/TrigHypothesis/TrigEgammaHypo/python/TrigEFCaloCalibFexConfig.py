# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# EF Calo Calib Fex Algorithm Configuration:
# Ryan Mackenzie White <ryan.white@cern.ch> 
##############################

#from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFCaloCalibFex
from TrigEgammaHypo import TrigEgammaHypoConf
from AthenaCommon.SystemOfUnits import GeV, mm

from AthenaCommon.AppMgr import ToolSvc
from egammaRec.Factories import ToolFactory

from egammaMVACalib import egammaMVACalibConf
egammaMVATool =  ToolFactory(egammaMVACalibConf.egammaMVATool,folder="egammaMVACalib/v1")
from egammaTools.egammaToolsFactories import EMFourMomBuilder, EMShowerBuilder 

def configureTrigEFCaloCalibFexMonitoring(tool):
    
    from TrigEgammaHypo.TrigEFCaloHypoMonitoring import TrigEFCaloCalibFexValidationMonitoring, TrigEFCaloCalibFexOnlineMonitoring
    validation = TrigEFCaloCalibFexValidationMonitoring()
    online     = TrigEFCaloCalibFexOnlineMonitoring()
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig("Time")

    tool.AthenaMonTools = [ time, validation, online ]

TrigEFCaloCalibFex_Electron = ToolFactory(TrigEgammaHypoConf.TrigEFCaloCalibFex, name = "TrigEFCaloCalibFex_Electron",
        AcceptAll = True,
        ApplyMVACalib = True,
        MVACalibTool = egammaMVATool,
        egType = 'Electron',
        ShowerBuilderTool = EMShowerBuilder(
                name = "TrigEgammaShowerBuilder",
                CellsName = "",
                Print = True,
                ),
        FourMomBuilderTool = EMFourMomBuilder(),
        postInit = [configureTrigEFCaloCalibFexMonitoring],
        )

TrigEFCaloCalibFex_Photon = ToolFactory(TrigEgammaHypoConf.TrigEFCaloCalibFex, name = "TrigEFCaloCalibFex_Photon",
        AcceptAll = True,
        ApplyMVACalib = True,
        MVACalibTool = egammaMVATool,
        egType = 'Photon',
        ShowerBuilderTool = EMShowerBuilder(
                name = "TrigEgammaShowerBuilder",
                CellsName = "",
                Print = True,
                ),
        FourMomBuilderTool = EMFourMomBuilder(),
        postInit = [configureTrigEFCaloCalibFexMonitoring],
        )
