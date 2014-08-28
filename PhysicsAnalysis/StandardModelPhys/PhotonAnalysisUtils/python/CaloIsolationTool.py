# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def CaloIsolationTool(DoJetAreaCorrections=True):
    
    from egammaCaloTools.egammaIsoPtCorrectionToolBase import egammaIsoPtCorrectionToolBase
    EMCaloIsoPtCorrectionTool = egammaIsoPtCorrectionToolBase("egammaIsoPtCorrection")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += EMCaloIsoPtCorrectionTool

    from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUcaloIsolationTool
    mycaloisolationtool = PAUcaloIsolationTool()
    mycaloisolationtool.DoAreaCorrections = DoJetAreaCorrections
    mycaloisolationtool.JetCollectionName = "Kt5TopoJets"
    mycaloisolationtool.UseOldPtCorrections = False
    mycaloisolationtool.UseOldAreaCorrections = True
    mycaloisolationtool.EMCaloIsoPtCorrectionTool = EMCaloIsoPtCorrectionTool

    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
    mycaloisolationtool.JetAreaMomentName = PAUflags.JetAreaMomentName()

    ToolSvc += mycaloisolationtool
    
    return mycaloisolationtool
