# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
import ROOT

def makeMuonAnalysisSequence( dataType, workingPoint = 'Medium',
                              deepCopyOutput = False ):
    """Create a muon analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if workingPoint == 'Tight' :
        quality = ROOT.xAOD.Muon.Tight
        pass
    elif workingPoint == 'Medium' :
        quality = ROOT.xAOD.Muon.Medium        
        pass
    elif workingPoint == 'Loose' :
        quality = ROOT.xAOD.Muon.Loose        
        pass
    elif workingPoint == 'VeryLoose' :
        quality = ROOT.xAOD.Muon.VeryLoose        
        pass
    elif workingPoint == 'HighPt' :
        quality = 4
        pass
    elif workingPoint == 'LowPtEfficiency' :
        quality = 5
        pass
    else :
        raise ValueError ("invalid working point: \"" + workingPoint +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose, HighPt, LowPtEfficiency")

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "MuonAnalysisSequence" )

    # Set up the muon calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg',
                           'MuonCalibrationAndSmearingAlg' )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::MuonCalibrationPeriodTool' )
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
    alg.selectionTool.MuQuality = quality
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
    alg.efficiencyScaleFactorTool.WorkingPoint = workingPoint
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                affectingSystematics = '(^MUON_EFF_.*)' )

    # Set up an algorithm dumping the properties of the muons, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'MuonKinematicDumperAlg' )
    alg.histPattern = 'muon_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'MuonDeepCopyMaker' )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output' )
        pass

    # Return the sequence:
    return seq
