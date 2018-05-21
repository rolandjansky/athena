# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
import ROOT

def makeMuonAnalysisSequence (dataType) :
    if not dataType in ["data", "mc", "afii"] :
        raise Exception ("invalid data type: " + dataType)

    sequence = []

    alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg', 'MuonCalibrationAndSmearingAlg' )
    addPrivateTool (alg, "calibrationAndSmearingTool", "CP::MuonCalibrationAndSmearingTool")
    sequence.append ( {"alg" : alg, "in" : "muons", "out" : "muonsOut",
                       "sys" : "(^MUON_ID$)|(^MUON_MS$)|(^MUON_SAGITTA_.*)|(^MUON_SCALE$)"} )


    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'MuonKinSelectionAlg' )
    addPrivateTool (alg, "selectionTool", "CP::AsgPtEtaSelectionTool")
    alg.selectionTool.minPt = 20e3
    alg.selectionTool.maxEta = 2.4
    alg.selectionDecoration = "kin_select"
    sequence.append ( {"alg" : alg, "in" : "particles", "out" : "particlesOut" } )

    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'MuonSelectionAlg' )
    addPrivateTool (alg, "selectionTool", "CP::MuonSelectionTool")
    alg.selectionDecoration = "good_muon"
    sequence.append ( {"alg" : alg, "in" : "particles", "out" : "particlesOut" } )

    alg = createAlgorithm( 'CP::MuonIsolationAlg', 'MuonIsolationAlg' )
    addPrivateTool (alg, "isolationTool", "CP::IsolationSelectionTool")
    alg.isolationDecoration = "isolated_muon"
    sequence.append ( {"alg" : alg, "in" : "muons", "out" : "muonsOut" } )



    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'MuonCutFlowDumperAlg' )
    alg.histPattern = "muon_cflow_%SYS%"
    alg.selection = ['kin_select', 'good_muon', 'isolated_muon']
    alg.selectionNCuts = [2,4,1]
    sequence.append ( {"alg" : alg, "in" : "input"} )



    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'MuonViewFromSelectionAlg' )
    alg.selection = ['kin_select', 'good_muon', 'isolated_muon']
    sequence.append ( {"alg" : alg, "in" : "input", "out" : "output", "needOut" : True} )



    alg = createAlgorithm( 'CP::MuonEfficiencyScaleFactorAlg', 'MuonEfficiencyScaleFactorAlg' )
    addPrivateTool (alg, "efficiencyScaleFactorTool", "CP::MuonEfficiencyScaleFactors")
    if dataType == "data" :
        alg.isData = 1
        pass
    else :
        alg.isData = 0
        pass
    alg.efficiencyDecoration = "muon_eff"
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = "bad_eff"
    sequence.append ( {"alg" : alg, "in" : "muons", "out" : "muonsOut",
                       "sys" : "(^MUON_EFF_.*)"} )



    alg = createAlgorithm( 'CP::KinematicHistAlg', 'MuonKinematicDumperAlg' )
    alg.histPattern = "muon_%VAR%_%SYS%"
    sequence.append ( {"alg" : alg, "in" : "input"} )


    return sequence
