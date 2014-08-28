# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PhotonNeuralNetworkTool as PNNTool

def PhotonNeuralNetworkTool():
    from AthenaCommon.AppMgr import ToolSvc

    if not hasattr(ToolSvc, "PhotonNeuralNetworkTool"):
        PhotonNNTool = PNNTool("PhotonNeuralNetworkTool",
                               Weights = "NeuralPhoton_TMVA")
        ToolSvc += PhotonNNTool
        return PhotonNNTool
    else:
        return ToolSvc.PhotonNeuralNetworkTool
