# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeMuonAnalysisSequence( dataType ):
    """Create a muon analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "MuonAnalysisSequence" )

    # Set up the muon calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg',
                           'MuonCalibrationAndSmearingAlg' )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::MuonCalibrationAndSmearingTool' )
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                affectingSystematics = '(^MUON_ID$)|(^MUON_MS$)|(^MUON_SAGITTA_.*)|(^MUON_SCALE$)' )

    # Set up the muon selection algorithm(s):
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'MuonKinSelectionAlg' )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.minPt = 20e3
    alg.selectionTool.maxEta = 2.4
    alg.selectionDecoration = 'kin_select'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    # Set up the track selection algorithm:
    alg = createAlgorithm( 'CP::AsgLeptonTrackSelectionAlg',
                           'MuonTrackSelectionAlg' )
    alg.selectionDecoration = "trackSelection"
    alg.maxD0Significance = 3
    alg.maxDeltaZ0SinTheta = 0.5
    seq.append( alg, inputPropName = 'particles', outputPropName = 'particlesOut' )

    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'MuonSelectionAlg' )
    addPrivateTool( alg, 'selectionTool', 'CP::MuonSelectionTool' )
    alg.selectionDecoration = 'good_muon'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    # Set up the isolation calculation algorithm:
    alg = createAlgorithm( 'CP::MuonIsolationAlg', 'MuonIsolationAlg' )
    addPrivateTool( alg, 'isolationTool', 'CP::IsolationSelectionTool' )
    alg.isolationDecoration = 'isolated_muon'
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut' )

    # Set up an algorithm used for debugging the muon selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'MuonCutFlowDumperAlg' )
    alg.histPattern = 'muon_cflow_%SYS%'
    alg.selection = [ 'kin_select', 'trackSelection', 'good_muon', 'isolated_muon' ]
    alg.selectionNCuts = [ 2, 2, 4, 1 ]
    seq.append( alg, inputPropName = 'input' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'MuonViewFromSelectionAlg' )
    alg.selection = [ 'kin_select', 'trackSelection', 'good_muon', 'isolated_muon' ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up the efficiency scale factor calculation algorithm:
    alg = createAlgorithm( 'CP::MuonEfficiencyScaleFactorAlg',
                           'MuonEfficiencyScaleFactorAlg' )
    addPrivateTool( alg, 'efficiencyScaleFactorTool',
                    'CP::MuonEfficiencyScaleFactors' )
    if dataType == "data":
        alg.isData = 1
        pass
    else :
        alg.isData = 0
        pass
    alg.efficiencyDecoration = 'muon_eff'
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff'
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                affectingSystematics = '(^MUON_EFF_.*)' )

    # Set up an algorithm dumping the properties of the muons, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'MuonKinematicDumperAlg' )
    alg.histPattern = 'muon_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input' )

    # Return the sequence:
    return seq
