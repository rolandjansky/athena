# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool, \
                                       createPublicTool

def makeTauAnalysisSequence( dataType, workingPoint,
                             deepCopyOutput = False, postfix = '' ):
    """Create a tau analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
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
    else :
        raise ValueError ("invalid tau quality: \"" + splitWP[0] +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose")

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "TauAnalysisSequence" + postfix )

    # Set up the tau 4-momentum smearing algorithm:
    alg = createAlgorithm( 'CP::TauSmearingAlg', 'TauSmearingAlg' + postfix )
    addPrivateTool( alg, 'smearingTool', 'TauAnalysisTools::TauSmearingTool' )
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                affectingSystematics = '(^TAUS_TRUEHADTAU_SME_TES_.*)',
                stageName = 'calibration' )

    # Set up the algorithm selecting "good" taus:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'TauSelectionAlg' + postfix )
    addPrivateTool( alg, 'selectionTool', 'TauAnalysisTools::TauSelectionTool' )
    alg.selectionTool.ConfigPath = inputfile
    alg.selectionDecoration = 'good_tau' + postfix
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection' )

    # Set up the algorithm calculating the efficiency scale factors for the
    # taus:
    alg = createAlgorithm( 'CP::TauEfficiencyCorrectionsAlg',
                           'TauEfficiencyCorrectionsAlg' + postfix )
    addPrivateTool( alg, 'efficiencyCorrectionsTool',
                    'TauAnalysisTools::TauEfficiencyCorrectionsTool' )
    selTool = createPublicTool( 'TauAnalysisTools::TauSelectionTool',
                                'TauSelectionTool')
    alg.efficiencyCorrectionsTool.TauSelectionTool = '%s/%s' % \
        ( selTool.getType(), selTool.getName() )
    seq.addPublicTool( selTool )
    alg.efficiencyDecoration = 'tau_eff' + postfix
    # alg.outOfValidity = 2 #silent
    # alg.outOfValidityDeco = "bad_eff"
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                affectingSystematics = '(^TAUS_TRUEELECTRON_EFF_.*)|(^TAUS_TRUEHADTAU_EFF_.*)',
                stageName = 'efficiency' )

    # Set up the tau truth matching algorithm:
    if dataType != 'data':
        alg = createAlgorithm( 'CP::TauTruthMatchingAlg',
                               'TauTruthMatchingAlg' + postfix )
        addPrivateTool( alg, 'matchingTool',
                        'TauAnalysisTools::TauTruthMatchingTool' )
        seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                    stageName = 'selection' )
        pass

    # Set up an algorithm used for debugging the tau selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'TauCutFlowDumperAlg' + postfix )
    alg.histPattern = 'tau_cflow_%SYS%'
    alg.selection = [ 'good_tau' + postfix ]
    alg.selectionNCuts = [ 6 ]
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'TauViewFromSelectionAlg' + postfix )
    alg.selection = [ 'good_tau' + postfix ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                stageName = 'selection' )

    # Set up an algorithm dumping the properties of the taus, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'TauKinematicDumperAlg' + postfix )
    alg.histPattern = "tau_%VAR%_%SYS%"
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'TauDeepCopyMaker' + postfix )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Return the sequence:
    return seq
