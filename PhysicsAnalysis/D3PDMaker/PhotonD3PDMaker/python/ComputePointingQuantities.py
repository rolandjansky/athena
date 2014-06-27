# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr      import ToolSvc

from egammaTools.EMClusterErrorsToolFactory import EMClusterErrorsToolFact
theemclustererrorstool=EMClusterErrorsToolFact("EMClusterErrorsTool")
ToolSvc += theemclustererrorstool

from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
thepauextrapolcaloconvCaloDepthTool = CaloDepthToolFactory(depth='cscopt2')
ToolSvc += thepauextrapolcaloconvCaloDepthTool

from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
theemextrapolcaloconvCaloDepthTool = CaloDepthToolFactory(depth='cscopt2')
ToolSvc += theemextrapolcaloconvCaloDepthTool
    
from egammaTrackTools.egammaTrackToolsConf import EMExtrapolCaloConversion
theemextrapolcaloconversion = EMExtrapolCaloConversion(CalorimeterDepth = theemextrapolcaloconvCaloDepthTool.getFullName())
ToolSvc += theemextrapolcaloconversion

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import ExtrapolCaloConversion
thepauextrapolcaloconversion = ExtrapolCaloConversion(CalorimeterDepth = thepauextrapolcaloconvCaloDepthTool.getFullName())
ToolSvc += thepauextrapolcaloconversion

from egammaCaloTools.egammaCaloToolsConf import egammaqpoint
theegammaqpoint=egammaqpoint("egammaQpoint",
                             CalorimeterDepth=thepauextrapolcaloconvCaloDepthTool.getName() )
theegammaqpoint += thepauextrapolcaloconvCaloDepthTool
ToolSvc+=theegammaqpoint 

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import ComputePointingQuantities
PAUComputePointingQuantities = ComputePointingQuantities( name = "PAUComputePointingQuantities")
PAUComputePointingQuantities.EMErrorDetailContainerName = "egDetailAOD"
PAUComputePointingQuantities.EMExtrapolCaloConversion  = theemextrapolcaloconversion
PAUComputePointingQuantities.PAUExtrapolCaloConversion = thepauextrapolcaloconversion
PAUComputePointingQuantities.EMClusterErrorsTool       = theemclustererrorstool
PAUComputePointingQuantities.CaloPointingTool          = theegammaqpoint
PAUComputePointingQuantities.CalorimeterDepth          = thepauextrapolcaloconvCaloDepthTool.getName()

ToolSvc += PAUComputePointingQuantities
    
