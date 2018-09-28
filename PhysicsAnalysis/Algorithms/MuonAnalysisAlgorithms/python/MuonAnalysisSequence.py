# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
import ROOT

def makeMuonAnalysisSequence( dataType, workingPoint,
                              deepCopyOutput = False, postfix = '' ):
    """Create a muon analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if postfix != '' :
        postfix = '_' + postfix
        pass

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
    seq = AnaAlgSequence( "MuonAnalysisSequence" + postfix )

    # Set up the muon calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg',
                           'MuonCalibrationAndSmearingAlg' + postfix )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::MuonCalibrationPeriodTool' )
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                affectingSystematics = '(^MUON_ID$)|(^MUON_MS$)|(^MUON_SAGITTA_.*)|(^MUON_SCALE$)',
                stageName = 'calibration' )

    # Set up the muon selection algorithm(s):
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'MuonKinSelectionAlg' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.minPt = 20e3
    alg.selectionTool.maxEta = 2.4
    alg.selectionDecoration = 'kin_select' + postfix
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection' )

    # Set up the track selection algorithm:
    alg = createAlgorithm( 'CP::AsgLeptonTrackSelectionAlg',
                           'MuonTrackSelectionAlg' + postfix )
    alg.selectionDecoration = "trackSelection" + postfix
    alg.maxD0Significance = 3
    alg.maxDeltaZ0SinTheta = 0.5
    seq.append( alg, inputPropName = 'particles', outputPropName = 'particlesOut',
                stageName = 'selection' )

    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'MuonSelectionAlg' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::MuonSelectionTool' )
    alg.selectionTool.MuQuality = quality
    alg.selectionDecoration = 'good_muon' + postfix
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection' )

    # Set up the isolation calculation algorithm:
    alg = createAlgorithm( 'CP::MuonIsolationAlg',
                           'MuonIsolationAlg' + postfix )
    addPrivateTool( alg, 'isolationTool', 'CP::IsolationSelectionTool' )
    alg.isolationDecoration = 'isolated_muon' + postfix
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                stageName = 'selection' )

    # Set up an algorithm used for debugging the muon selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                           'MuonCutFlowDumperAlg' + postfix )
    alg.histPattern = 'muon' + postfix + '_cflow_%SYS%'
    alg.selection = [ 'kin_select' + postfix, 'trackSelection' + postfix, 'good_muon' + postfix, 'isolated_muon' + postfix ]
    alg.selectionNCuts = [ 2, 2, 4, 1 ]
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'MuonViewFromSelectionAlg' + postfix )
    alg.selection = [ 'kin_select' + postfix, 'trackSelection' + postfix, 'good_muon' + postfix, 'isolated_muon' + postfix ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                stageName = 'selection' )

    # Set up the efficiency scale factor calculation algorithm:
    alg = createAlgorithm( 'CP::MuonEfficiencyScaleFactorAlg',
                           'MuonEfficiencyScaleFactorAlg' + postfix )
    addPrivateTool( alg, 'efficiencyScaleFactorTool',
                    'CP::MuonEfficiencyScaleFactors' )
    if dataType == "data":
        alg.isData = 1
        pass
    else :
        alg.isData = 0
        pass
    alg.efficiencyDecoration = 'muon_eff' + postfix
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff' + postfix
    alg.efficiencyScaleFactorTool.WorkingPoint = workingPoint
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                affectingSystematics = '(^MUON_EFF_.*)',
                stageName = 'efficiency' )

    # Set up an algorithm dumping the properties of the muons, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg',
                           'MuonKinematicDumperAlg' + postfix )
    alg.histPattern = 'muon' + postfix + '_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'MuonDeepCopyMaker' + postfix )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Return the sequence:
    return seq
