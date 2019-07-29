# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
import ROOT

def makeMuonAnalysisSequence( dataType, workingPoint,
                              deepCopyOutput = False,
                              shallowViewOutput = True,
                              postfix = '',
                              ptSelectionOutput = False ):
    """Create a muon analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      shallowViewOutput -- Create a view container if required
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      ptSelectionOutput -- Whether or not to apply pt selection when creating
                           output containers.
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if postfix != '' :
        postfix = '_' + postfix
        pass

    # Make sure selection options make sense
    if deepCopyOutput and shallowViewOutput:
        raise ValueError ("deepCopyOutput and shallowViewOutput can't both be true!")

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 2 :
        raise ValueError ('working point should be of format "quality.isolation", not ' + workingPoint)

    sfWorkingPoint = splitWP[0]
    if splitWP[0] == 'Tight' :
        quality = ROOT.xAOD.Muon.Tight
        pass
    elif splitWP[0] == 'Medium' :
        quality = ROOT.xAOD.Muon.Medium        
        pass
    elif splitWP[0] == 'Loose' :
        quality = ROOT.xAOD.Muon.Loose        
        pass
    elif splitWP[0] == 'VeryLoose' :
        quality = ROOT.xAOD.Muon.VeryLoose        
        pass
    elif splitWP[0] == 'HighPt' :
        quality = 4
        pass
    elif splitWP[0] == 'LowPtEfficiency' :
        quality = 5
        pass
    else :
        raise ValueError ("invalid muon quality: \"" + splitWP[0] +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose, HighPt, LowPtEfficiency")

    if not splitWP[1] in ["Iso", "NonIso"] :
        raise ValueError ('invalid muon isolation \"' + splitWP[1] +
                          '\", allowed values are Iso, NonIso')

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "MuonAnalysisSequence" + postfix )

    # Variables keeping track of the selections being applied.
    selectionDecorNames = []
    selectionDecorCount = []

    # Set up the eta-cut on all muons prior to everything else
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'MuonEtaCutAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.maxEta = 2.5
    alg.selectionDecoration = 'selectEta' + postfix + ',as_bits'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 2 )

    # Set up the muon calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg',
                           'MuonCalibrationAndSmearingAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::MuonCalibrationPeriodTool' )
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                affectingSystematics = '(^MUON_ID$)|(^MUON_MS$)|(^MUON_SAGITTA_.*)|(^MUON_SCALE$)',
                stageName = 'calibration' )

    # Set up the the pt selection
    ptSelectionDecoration = 'selectPt' + postfix + ',as_bits'
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'MuonPtCutAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    alg.selectionDecoration = ptSelectionDecoration
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.minPt = 3e3
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 2 )

    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'MuonQualitySelectionAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    addPrivateTool( alg, 'selectionTool', 'CP::MuonSelectionTool' )
    alg.selectionTool.MuQuality = quality
    alg.selectionDecoration = 'good_muon' + postfix + ',as_bits'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 4 )

    # Set up the track selection algorithm:
    # TODO: temporarily moved after the quality selection due to ATLASG-780
    alg = createAlgorithm( 'CP::AsgLeptonTrackSelectionAlg',
                           'MuonTrackSelectionAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    alg.selectionDecoration = 'trackSelection' + postfix + ',as_bits'
    alg.maxD0Significance = 3
    alg.maxDeltaZ0SinTheta = 0.5
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 3 )

    # Set up the isolation calculation algorithm:
    if splitWP[1] != 'NonIso' :
        alg = createAlgorithm( 'CP::MuonIsolationAlg',
                               'MuonIsolationAlg' + postfix )
        alg.preselection = "&&".join (selectionDecorNames)
        addPrivateTool( alg, 'isolationTool', 'CP::IsolationSelectionTool' )
        alg.isolationDecoration = 'isolated_muon' + postfix + ',as_bits'
        seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                    stageName = 'selection' )
        selectionDecorNames.append( alg.isolationDecoration )
        selectionDecorCount.append( 1 )
        pass

    # Set up an algorithm used for decorating baseline muon selection:
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'MuonSelectionSummary' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
    alg.selectionTool.selectionFlags = selectionDecorNames[ : ]
    alg.selectionDecoration = 'baselineSelection' + postfix + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )

    # Set up an algorithm used for debugging the muon selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                           'MuonCutFlowDumperAlg' + postfix )
    alg.histPattern = 'muon' + postfix + '_cflow_%SYS%'
    alg.selection = selectionDecorNames[ : ]
    alg.selectionNCuts = selectionDecorCount[ : ]
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up the output selection
    if shallowViewOutput or deepCopyOutput:
        selectionDecorNamesOutput = selectionDecorNames[ : ]
        if not ptSelectionOutput:
            selectionDecorNamesOutput.remove(ptSelectionDecoration)

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    if shallowViewOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                            'MuonViewFromSelectionAlg' + postfix )
        alg.selection = selectionDecorNames[ : ]
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )

    # Set up the efficiency scale factor calculation algorithm:
    alg = createAlgorithm( 'CP::MuonEfficiencyScaleFactorAlg',
                           'MuonEfficiencyScaleFactorAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    addPrivateTool( alg, 'efficiencyScaleFactorTool',
                    'CP::MuonEfficiencyScaleFactors' )
    alg.scaleFactorDecoration = 'muon_effSF' + postfix + "_%SYS%"
    alg.scaleFactorDecorationRegex = '(^MUON_EFF_RECO.*)'
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff' + postfix
    alg.efficiencyScaleFactorTool.WorkingPoint = sfWorkingPoint
    if dataType != 'data':
        seq.append( alg, inputPropName = 'muons',
                    affectingSystematics = '(^MUON_EFF_RECO.*)',
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
