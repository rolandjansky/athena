# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeMuonAnalysisSequence (dataType) :
    if not dataType in ["data", "mc", "afii"] :
        raise Exception ("invalid data type: " + dataType)

    sequence = []

    alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg', 'MuonCalibrationAndSmearingAlg' )
    addPrivateTool (alg, "calibrationAndSmearingTool", "CP::MuonCalibrationAndSmearingTool")
    sequence.append ( {"alg" : alg, "in" : "muons", "out" : "muonsOut",
                       "sys" : "(^MUON_ID$)|(^MUON_MS$)|(^MUON_SAGITTA_.*)|(^MUON_SCALE$)"} )


    alg = createAlgorithm( 'CP::MuonSelectionAlg', 'MuonSelectionAlg' )
    addPrivateTool (alg, "selectionTool", "CP::MuonSelectionTool")
    alg.selectionDecoration = "good_muon"
    sequence.append ( {"alg" : alg, "in" : "muons", "out" : "muonsOut" } )

    alg = createAlgorithm( 'CP::MuonIsolationAlg', 'MuonIsolationAlg' )
    addPrivateTool (alg, "isolationTool", "CP::IsolationSelectionTool")
    alg.isolationDecoration = "isolated_muon"
    sequence.append ( {"alg" : alg, "in" : "muons", "out" : "muonsOut" } )

    return sequence
