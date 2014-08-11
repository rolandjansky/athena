# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" TrigTauRec """

__author__  = 'S.Xella, O.Igonkina, F.Friedrich'
__version__ =""
__doc__     ="Configuration of TrigTauRec"

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from TrigTauRec.TrigTauRecConf import TrigTauRecMerged

from AthenaCommon.AppMgr import ToolSvc

class TrigTauRecMerged_Tau (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_Tau"):
            super( TrigTauRecMerged_Tau , self ).__init__( name )
            self._mytools = []

            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
            
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

           
            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            #what should run in trigger???
            #same as tauRec (but without Pi0 cluster finder)
            tools.append(taualgs.getJetSeedBuilder())
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            tools.append(taualgs.getTauAxis())
            tools.append(taualgs.getTauTrackFinder())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            tools.append(taualgs.getCellVariables(cellConeSize=0.4))
            
            tools.append(taualgs.getElectronVetoVars())  #needed?
            #tools.append(taualgs.getEnergyCalibrationEM())
            ## tools.append(taualgs.getTauEflowTrackMatchCells())
            ## tools.append(taualgs.getTauEflowAddCaloInfo())  
            ## tools.append(taualgs.getTauEflowVariables())
            
            tools.append(taualgs.getTauVertexVariables())
            tools.append(taualgs.getTauCommonCalcVars())
            tools.append(taualgs.getTauSubstructure())
            #tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            
            self.Tools = tools

class TrigTauRecMerged_Tau2012 (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_Tau2012"):
            super( TrigTauRecMerged_Tau2012 , self ).__init__( name )
            self._mytools = []

            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
            
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

           
            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            #taualgs.setPrefix("TrigTau2012_")

            #what should run in trigger???
            #same as tauRec (but without Pi0 cluster finder)
            tools.append(taualgs.getJetSeedBuilder())
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            tools.append(taualgs.getTauAxis())
            tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=2, prefix="TrigTau2012_"))
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))

            tools.append(taualgs.getCellVariables(cellConeSize=0.2, prefix="TrigTau2012_"))  #cellConeSize 0.2!!

            tools.append(taualgs.getElectronVetoVars())  #needed?
            tools.append(taualgs.getTauVertexVariables())
            tools.append(taualgs.getTauCommonCalcVars())
            tools.append(taualgs.getTauSubstructure())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            
            self.Tools = tools

            #necessary to write out deltaZ0 between tracks and lead trk
            #self.theTauPVTool = taualgs.getTauPVTrackTool()
            #self.useTauPVTool = True;
            
            
#end
