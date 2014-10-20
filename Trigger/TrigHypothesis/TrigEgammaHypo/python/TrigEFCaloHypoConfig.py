# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

############################################
## EF Calo Hypo Configuration
## Ryan Mackenzie White <ryan.white@cern.ch>
############################################
from AthenaCommon.Logging import logging #AT
import traceback #AT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFCaloHypo
from AthenaCommon.SystemOfUnits import GeV
from egammaRec.Factories import ToolFactory
import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import egammaPID
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu
class TrigEFCaloHypoBase (TrigEFCaloHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFCaloHypoBase, self ).__init__( name )

        #-----------------------------------------------------------
        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'TrigEFCaloHypoBase:' )
        from egammaCaloTools.egammaCaloToolsFactories import egammaShowerShape
       
        self.ShowerShapeTool = egammaShowerShape()       
        try:
            from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
        except:
            mlog = logging.getLogger(name+'::__init__')
            mlog.error("could not get handle to AsgElectronSelectorTool")
            print traceback.format_exc()
            return False
        if hasattr(ToolSvc, "AsgSelectorTool"):
            asgtool=ConfiguredAsgElectronIsEMSelector("AsgSelectorTool",egammaPID.PhotonIDLooseEF,electronPIDmenu.menuTrig2012)
            self.SelectorTool = asgtool
            self.SelectorToolName = asgtool.getFullName()
        else:    
            asgtool=ConfiguredAsgElectronIsEMSelector("AsgSelectorTool",egammaPID.PhotonIDLooseEF,electronPIDmenu.menuTrig2012)
            ToolSvc+=asgtool
            self.SelectorTool = asgtool
            self.SelectorToolName = asgtool.getFullName()

class TrigEFCaloHypo_All (TrigEFCaloHypoBase):       
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFCaloHypo_All, self ).__init__( name )
        self.UseShowerShapeTool=True
        self.Etcut = float(threshold)*GeV
        self.AcceptAll = True

        
    
