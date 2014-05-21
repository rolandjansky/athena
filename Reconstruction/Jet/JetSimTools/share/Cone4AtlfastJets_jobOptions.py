#----------------------------------------------------------------------
#      jobOptions for JetRec package
#      Cone (DR 0.4) jet reconstruction for Atlfast
#----------------------------------------------------------------------

# Include the relevant calibrations

include ( "JetSimTools/AtlfastJetCalibrationParameters.py" )

#----------------------------------------------------------------------
# DLL Libraries
#----------------------------------------------------------------------
theApp.Dlls += [ "JetRec" ]
theApp.Dlls += [ "CaloUtils"]
#----------------------------------------------------------------------
# Algorithm steering
#----------------------------------------------------------------------
theApp.topAlg += [ "JetAlgorithm/Cone4AtlJets" ]

Cone4AtlJets = Algorithm( "Cone4AtlJets" )

Cone4AtlJets.InputCollectionNames = [ "/Event/AtlfastCellsIN4M" ]

# Cone4AtlJets configuration

Cone4AtlJets.JetCollectionName = "Cone4AtlfastJets"
Cone4AtlJets.AlgTools = [
    "JetConeFinderTool/ConeFinderAtl",
    "JetSplitMergeTool/SplitMergeAtl",
    "JetAtlfastCalibratorTool/Calib",
    "JetSignalSelectorTool/FinalEtCutAtl" ]

# -- JetConeFinderTool

Cone4AtlJets.ConeFinderAtl.ConeR    = 0.4
Cone4AtlJets.ConeFinderAtl.SeedPt   = 2*GeV

# -- Initial signal selection

Cone4AtlJets.Calib.ScaleFactor = atlfast_scalefactor_cone04
Cone4AtlJets.Calib.InputParameters = atlfast_h1_parameters_cone04
Cone4AtlJets.Calib.CalibrationAlgorithm = "H1"


# -- Calibration

# -- Final signal selection

Cone4AtlJets.FinalEtCutAtl.UseTransverseEnergy = True
Cone4AtlJets.FinalEtCutAtl.MinimumSignal       = 10.*GeV


