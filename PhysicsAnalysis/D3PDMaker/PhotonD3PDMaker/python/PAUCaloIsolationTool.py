# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Constants import WARNING, INFO, ERROR, VERBOSE
from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
from AthenaCommon.AppMgr      import ToolSvc

# Calorimeter isolation tool for truth (hadron-level) isolation
from egammaCaloTools.egammaIsoPtCorrectionToolBase import egammaIsoPtCorrectionToolBase
EMCaloIsoPtCorrectionTool = egammaIsoPtCorrectionToolBase("egammaIsoPtCorrection")
ToolSvc += EMCaloIsoPtCorrectionTool

from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUcaloIsolationTool
PAUCaloIsolationTool = PAUcaloIsolationTool("PAUCaloIsolationTool")
PAUCaloIsolationTool.DoAreaCorrections = PAUflags.DoReRunJetFinding()
PAUCaloIsolationTool.JetCollectionName = "Kt5TopoJets"
PAUCaloIsolationTool.JetAreaEtaBins    = PAUflags.JetAreaEtaBins()
PAUCaloIsolationTool.UseOldPtCorrections = PAUflags.UseOldPtCorrections()
PAUCaloIsolationTool.UseOldAreaCorrections = True
PAUCaloIsolationTool.EMCaloIsoPtCorrectionTool = EMCaloIsoPtCorrectionTool
PAUCaloIsolationTool.OutputLevel       = WARNING
PAUCaloIsolationTool.JetAreaMomentName = PAUflags.JetAreaMomentName()

ToolSvc += PAUCaloIsolationTool
