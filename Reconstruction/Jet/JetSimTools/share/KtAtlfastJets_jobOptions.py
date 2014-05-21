#----------------------------------------------------------------------
#      jobOptions for JetRec package
#      Kt jet reconstruction for Atlfast
#----------------------------------------------------------------------

# This is to include the relevant calibration parameters

include ( "JetSimTools/AtlfastJetCalibrationParameters.py" )

#----------------------------------------------------------------------
# DLL Libraries
#----------------------------------------------------------------------
theApp.Dlls += [ "JetRec" ]
theApp.Dlls += [ "CaloUtils"]
#----------------------------------------------------------------------
# Algorithm steering
#----------------------------------------------------------------------
theApp.topAlg += [ "JetAlgorithm/KtAtlJets" ]

KtAtlJets = Algorithm( "KtAtlJets" )

# Input container - Retrieve Atlfast cells
# WARNING!!!!! This may change 

KtAtlJets.InputCollectionNames = [ "/Event/AtlfastCellsIN4M" ]

# KtAtlJets configuration

KtAtlJets.JetCollectionName = "KtAtlfastJets"
KtAtlJets.AlgTools = [
    "JetFastKtFinderTool/KtFinderAtl",
    "JetSplitMergeTool/SplitMergeAtl",
    "JetAtlfastCalibrator/Calib",
    "JetSignalSelectorTool/FinalEtCutAtl" ]

# -- JetKtFinderTool

KtAtlJets.KtFinderAtl.BeamType = "PP"
KtAtlJets.KtFinderAtl.DistScheme = "DeltaR"
KtAtlJets.KtFinderAtl.RecomScheme = "E"
KtAtlJets.KtFinderAtl.RParameter = 1.0

#KtAtlJets.Calib.ScaleFactor = atlfast_scalefactor_cone04
KtAtlJets.Calib.InputParameters = atlfast_h1_parameters_kt
KtAtlJets.Calib.CalibrationAlgorithm = "H1"
# -- Calibration

# -- Final signal selection

KtAtlJets.FinalEtCutAtl.UseTransverseEnergy = True
KtAtlJets.FinalEtCutAtl.MinimumSignal       = 10.*GeV


