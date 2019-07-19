# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool, \
                                       createPublicTool

def makeTauAnalysisSequence( dataType, workingPoint,
                             deepCopyOutput = False,
                             shallowViewOutput = True,
                             rerunTruthMatching = True,
                             postfix = '' ):
    """Create a tau analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      shallowViewOutput -- Create a view container if required
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

    # Make sure selection options make sense
    if deepCopyOutput and shallowViewOutput:
        raise ValueError ("deepCopyOutput and shallowViewOutput can't both be true!")

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 1 :
        raise ValueError ('working point should be of format "quality", not ' + workingPoint)

    sfWorkingPoint = splitWP[0]
    if splitWP[0] == 'Tight' :
        inputfile = 'TauAnalysisAlgorithms/tau_selection_tight.conf'
        pass
    elif splitWP[0] == 'Medium' :
        inputfile = 'TauAnalysisAlgorithms/tau_selection_medium.conf'
        pass
    elif splitWP[0] == 'Loose' :
        inputfile = 'TauAnalysisAlgorithms/tau_selection_loose.conf'
        pass
    elif splitWP[0] == 'VeryLoose' :
        inputfile = 'TauAnalysisAlgorithms/tau_selection_veryloose.conf'
        pass
    elif splitWP[0] == 'NoID' :
        inputfile = 'TauAnalysisAlgorithms/tau_selection_noid.conf'
        pass
    elif splitWP[0] == 'Baseline' :
        inputfile = 'TauAnalysisAlgorithms/tau_selection_baseline.conf'
        pass
    else :
        raise ValueError ("invalid tau quality: \"" + splitWP[0] +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose, NoID, Baseline")

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "TauAnalysisSequence" + postfix )

    # Variables keeping track of the selections being applied.
    selectionDecorNames = []
    selectionDecorCount = []

    # Setup the tau selection tool
    selectionTool = createPublicTool( 'TauAnalysisTools::TauSelectionTool',
                                      'TauSelectionTool')
    selectionTool.ConfigPath = inputfile
    seq.addPublicTool( selectionTool )

    # Set up the tau truth matching algorithm:
    if rerunTruthMatching and dataType != 'data':
        alg = createAlgorithm( 'CP::TauTruthMatchingAlg',
                               'TauTruthMatchingAlg' + postfix )
        addPrivateTool( alg, 'matchingTool',
                        'TauAnalysisTools::TauTruthMatchingTool' )
        alg.matchingTool.WriteTruthTaus = 1
        seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                    stageName = 'selection' )
        pass

    # Set up the tau 4-momentum smearing algorithm:
    alg = createAlgorithm( 'CP::TauSmearingAlg', 'TauSmearingAlg' + postfix )
    addPrivateTool( alg, 'smearingTool', 'TauAnalysisTools::TauSmearingTool' )
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                affectingSystematics = '(^TAUS_TRUEHADTAU_SME_TES_.*)',
                stageName = 'calibration' )

    # Set up the algorithm selecting "good" taus:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'TauSelectionAlg' + postfix )
    alg.selectionTool = '%s/%s' % \
        ( selectionTool.getType(), selectionTool.getName() )
    alg.selectionDecoration = 'good_tau' + postfix + ',as_bits'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 6 )

    # Set up the algorithm calculating the efficiency scale factors for the
    # taus:
    if dataType != 'data':
        alg = createAlgorithm( 'CP::TauEfficiencyCorrectionsAlg',
                            'TauEfficiencyCorrectionsAlg' + postfix )
        addPrivateTool( alg, 'efficiencyCorrectionsTool',
                        'TauAnalysisTools::TauEfficiencyCorrectionsTool' )
        alg.efficiencyCorrectionsTool.TauSelectionTool = '%s/%s' % \
            ( selectionTool.getType(), selectionTool.getName() )
        alg.scaleFactorDecoration = 'tau_effSF' + postfix
        alg.outOfValidity = 2 #silent
        alg.outOfValidityDeco = 'bad_eff' + postfix
        seq.append( alg, inputPropName = 'taus',
                    affectingSystematics = '(^TAUS_TRUEELECTRON_EFF_.*)|(^TAUS_TRUEHADTAU_EFF_.*)',
                    stageName = 'efficiency' )

    # Set up an algorithm used for debugging the tau selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'TauCutFlowDumperAlg' + postfix )
    alg.histPattern = 'tau_cflow_%SYS%'
    alg.selection = selectionDecorNames[ : ]
    alg.selectionNCuts = selectionDecorCount[ : ]
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up an algorithm used for decorating baseline tau selection:
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'TauSelectionSummary' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
    alg.selectionTool.selectionFlags = selectionDecorNames[ : ]
    alg.selectionDecoration = 'baselineSelection' + postfix + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    if shallowViewOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                            'TauViewFromSelectionAlg' + postfix )
        alg.selection = selectionDecorNames[ : ]
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )

    # Set up an algorithm dumping the properties of the taus, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'TauKinematicDumperAlg' + postfix )
    alg.preselection = '&&'.join (selectionDecorNames)
    alg.histPattern = 'tau_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'TauDeepCopyMaker' + postfix )
        alg.deepCopy = True
        alg.selection = selectionDecorNames[ : ]
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Return the sequence:
    return seq
