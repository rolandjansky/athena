# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AppMgr import ToolSvc
from HIGlobal.HIGlobalConf import HIEventShapeFillerTool
__fillerTool = HIEventShapeFillerTool("TriggerHIEshapeFillerTool")
ToolSvc += __fillerTool

from TrigHIHypo.TrigHIHypoConf import CreateHIUEEstimate
theUEMaker                       = CreateHIUEEstimate("ueMaker")


from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_fullcalo
theFSCellMaker                   = TrigCaloCellMaker_fullcalo("TrigCaloCellMakerFullCalo_eGamma_heavyIon")


from TrigHIHypo.TrigHIHypoConf import HICaloCellCorectionTool #, CreateHIUEEstimate, 
__cellsCorrector = HICaloCellCorectionTool("cellsCorrector")
__cellsCorrector.HIEventShapeContainerName="HLT_xAOD__HIEventShapeContainer_HIUE"
ToolSvc += __cellsCorrector

from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_eGamma
theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma("TrigCaloCellMaker_eGamma_heavyIon")
theTrigCaloCellMaker_eGamma.ContainerTools += [ __cellsCorrector.getFullName() ]



#from code import interact
#interact("TrigCaloRecModifier>> ", local=locals())

#print ToolSvc


#from sys import exit
#print ".... exiting"
#exit(0)

