#----------------------------------------------------------------------
#      jobOptions for JetRec package
#      Cone (DR 0.4) jet reconstruction for Atlfast
#----------------------------------------------------------------------
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

# Input container - Retrieve Atlfast cells
# WARNING!!!!! This may change 

Cone4AtlJets.InputCollectionNames = [ "/Event/AtlfastCellsIN4M" ]

# Cone4AtlJets configuration

Cone4AtlJets.JetCollectionName = "Cone4AtlfastJets"
Cone4AtlJets.AlgTools = [
    "JetSignalSelectorTool/InitialEtCutAtl;",
    "JetConeFinderTool/ConeFinderAtl",
    "JetSplitMergeTool/SplitMergeAtl",
    "JetAtlfastCalibrator/SamplingCalib",
    "JetSignalSelectorTool/FinalEtCutAtl" ]

# -- JetConeFinderTool

Cone4AtlJets.ConeFinderAtl.ConeR    = 0.4
Cone4AtlJets.ConeFinderAtl.SeedPt   = 2*GeV

# -- Initial signal selection

Cone4AtlJets.InitialEtCutAtl.UseTransverseEnergy = True
Cone4AtlJets.InitialEtCutAtl.MinimumSignal       = 0*MeV
Cone4AtlJets.SamplingCalib.InputParameters = atlfast_sampling_parameters
Cone4AtlJets.SamplingCalib.CalibrationAlgorithm = "sampling"


# -- Calibration

# -- Final signal selection

Cone4AtlJets.FinalEtCutAtl.UseTransverseEnergy = True
Cone4AtlJets.FinalEtCutAtl.MinimumSignal       = 10.*GeV


