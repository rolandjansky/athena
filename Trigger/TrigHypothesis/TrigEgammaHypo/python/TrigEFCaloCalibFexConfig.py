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
egammaMVATool =  ToolFactory(egammaMVACalibConf.egammaMVATool)
from egammaTools.egammaToolsFactories import EMFourMomBuilder, EMShowerBuilder 
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
        )
# ---------------------------------------------------------------
# class for common setups (like monitoring)
#class TrigEFCaloCalibFexBase(TrigEFCaloCalibFex):
#    __slots__ = []
#    def __init__(self, name):
#        super(TrigEFCaloCalibFex,self).__init__(name)
#        self.AcceptAll = True
#        self.MVACalibTool = egammaMVATool()
#        self.egType = 'Electron'

# ---------------------------------------------------------------
# TrigEFCaloCalibFex configurations
# ---------------------------------------------------------------

#class TrigEFCaloCalibFex_Electron(TrigEFCaloCalibFexBase):
#    __slots__ = []
#    def __init__(self,name="TrigEFCaloCalibFex_Electron"):
#        super(TrigEFCaloCalibFex_Electron, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
#        self.AcceptAll = False
#        self.egType = 'Electron'
#        
#class TrigEFCaloCalibFex_Photon(TrigEFCaloCalibFexBase):
#    __slots__ = []
#    def __init__(self,name="TrigEFCaloCalibFex_Photon"):
#        super(TrigEFCaloCalibFex_Electron, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
#        self.AcceptAll = False
#        self.egType = 'Photon'
