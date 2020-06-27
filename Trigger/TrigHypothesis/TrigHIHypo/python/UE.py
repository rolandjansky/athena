# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AppMgr import ToolSvc
from HIGlobal.HIGlobalConf import HIEventShapeFillerTool
__fillerTool = HIEventShapeFillerTool("HIEventShapeFillerTool")
ToolSvc += __fillerTool

from TrigHIHypo.TrigHIHypoConf import CreateHIUEEstimate
theUEMaker                       = CreateHIUEEstimate("ueMaker")


from TrigHIHypo.TrigHIHypoConf import ElectronUEMonitoring
theElectronUEMonitoring = ElectronUEMonitoring("ElectronUEMonitoring")



from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_fullcalo
theFSCellMaker                   = TrigCaloCellMaker_fullcalo("TrigCaloCellMakerFullCalo_eGamma_heavyIon")

from TrigHIHypo.TrigHIHypoConf import HICaloCellCorectionTool #, CreateHIUEEstimate, 
__cellsCorrector = HICaloCellCorectionTool("cellsCorrector")
__cellsCorrector.HIEventShapeContainerName="HLT_xAOD__HIEventShapeContainer_HIUE"
ToolSvc += __cellsCorrector

from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_eGamma
theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma("TrigCaloCellMaker_eGamma_heavyIon")
theTrigCaloCellMaker_eGamma.PersistencyKeyName="TrigCaloCellMaker_HIUE"
theTrigCaloCellMaker_eGamma.PersistencyPrescaleFactor=1
theTrigCaloCellMaker_eGamma.ContainerTools += [ __cellsCorrector.getFullName() ]

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 
class ElectronUEMonitoringTool(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="ElectronUEMonitoringTool"):
        """ defines histograms for online """
        super(TrigGenericMonitoringToolConfig, self).__init__(name)
        
        self.defineTarget(["Online","Validation"])
        
        self.Histograms += [ defineHistogram ('FcalEt, EtDiffLow' ,type='TH2F',title='ElectronUEMonitoring Et_EF - Et_L2 (low Et) ; Fcal_Et [TeV]; Et_EF - Et_L2 [GeV]' ,ybins=100,ymin=-20.,ymax=20,xbins=100,xmin=0.,xmax=5.)]
        
        self.Histograms += [ defineHistogram ('FcalEt, EtDiffMedium' ,type='TH2F',title='ElectronUEMonitoring Et_EF - Et_L2 (medium Et) ; Fcal_Et [TeV]; Et_EF - Et_L2 [GeV]' ,ybins=100,ymin=-20.,ymax=20,xbins=100,xmin=0.,xmax=5.)]
        
        self.Histograms += [ defineHistogram ('FcalEt, EtDiffHigh' ,type='TH2F',title='ElectronUEMonitoring Et_EF - Et_L2 (high Et); Fcal_Et [TeV]; Et_EF - Et_L2 [GeV] ' ,ybins=100,ymin=-20.,ymax=20,xbins=100,xmin=0.,xmax=5.)]

        
from TrigHIHypo.TrigHIHypoConf import ElectronUEMonitoring
theElectronUEMonitoring = ElectronUEMonitoring("EgammaUEMonitoring")
theElectronUEMonitoring.AthenaMonTools = [ElectronUEMonitoringTool()]


#from code import interact
#interact("TrigCaloRecModifier>> ", local=locals())

#print ToolSvc


#from sys import exit
#print ".... exiting"
#exit(0)

