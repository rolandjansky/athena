#----------------------------------------------------------------------
#      jobOptions for JetRec package
#      Cone (DR 0.7) jet reconstruction for Atlfast
#----------------------------------------------------------------------

# This is to include the relevant calibration vectors

include ( "JetSimTools/AtlfastJetCalibrationParameters.py" )

#----------------------------------------------------------------------
# DLL Libraries
#----------------------------------------------------------------------
theApp.Dlls += [ "JetRec" ]
theApp.Dlls += [ "CaloUtils"]
#----------------------------------------------------------------------
# Algorithm steering
#----------------------------------------------------------------------
theApp.topAlg += [ "JetAlgorithm/ConeAtlJets" ]

ConeAtlJets = Algorithm( "ConeAtlJets" )

# Input container - Retrieve Atlfast cells
# WARNING!!!!! This may change 

ConeAtlJets.InputCollectionNames = [ "/Event/AtlfastCellsIN4M" ]

# ConeAtlJets configuration

ConeAtlJets.JetCollectionName = "ConeAtlfastJets"
ConeAtlJets.AlgTools = [
    "JetConeFinderTool/ConeFinderAtl",
    "JetSplitMergeTool/SplitMergeAtl",
    "JetAtlfastCalibrator/Calib",
    "JetSignalSelectorTool/FinalEtCutAtl" ]

# -- JetConeFinderTool

ConeAtlJets.ConeFinderAtl.ConeR    = 0.7
ConeAtlJets.ConeFinderAtl.SeedPt   = 2*GeV

# -- Initial signal selection

#ConeAtlJets.Calib.ScaleFactor = atlfast_scalefactor_cone04
ConeAtlJets.Calib.InputParameters = atlfast_h1_parameters_cone07
ConeAtlJets.Calib.CalibrationAlgorithm = "H1"



# -- Calibration

# -- Final signal selection

ConeAtlJets.FinalEtCutAtl.UseTransverseEnergy = True
ConeAtlJets.FinalEtCutAtl.MinimumSignal       = 10.*GeV


