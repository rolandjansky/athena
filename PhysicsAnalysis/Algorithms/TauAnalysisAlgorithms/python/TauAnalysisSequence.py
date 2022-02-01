# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AsgAnalysisAlgorithms.AnalysisObjectSharedSequence import makeSharedObjectSequence
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool, \
                                       createPublicTool

def makeTauAnalysisSequence( dataType, workingPoint, postfix = '',
                             legacyRecommendations = False,
                             deepCopyOutput = False,
                             shallowViewOutput = True,
                             rerunTruthMatching = True,
                             enableCutflow = False,
                             enableKinematicHistograms = False ):
    """Create a tau analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      legacyRecommendations -- use legacy tau BDT and electron veto recommendations
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      shallowViewOutput -- Create a view container if required
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      rerunTruthMatching -- Whether or not to rerun truth matching
      enableCutflow -- Whether or not to dump the cutflow
      enableKinematicHistograms -- Whether or not to dump the kinematic histograms
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if postfix != '' :
        postfix = '_' + postfix
        pass

    # Make sure selection options make sense
    if deepCopyOutput and shallowViewOutput:
        raise ValueError ("deepCopyOutput and shallowViewOutput can't both be true!")

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "TauAnalysisSequence" + postfix )

    seq.addMetaConfigDefault ("selectionDecorNames", [])
    seq.addMetaConfigDefault ("selectionDecorNamesOutput", [])
    seq.addMetaConfigDefault ("selectionDecorCount", [])

    makeTauCalibrationSequence (seq, dataType, postfix=postfix,
                                rerunTruthMatching = rerunTruthMatching)
    makeTauWorkingPointSequence (seq, dataType, workingPoint, postfix=postfix,
                                 legacyRecommendations = legacyRecommendations)
    makeSharedObjectSequence (seq, deepCopyOutput = deepCopyOutput,
                              shallowViewOutput = shallowViewOutput,
                              postfix = '_Tau' + postfix,
                              enableCutflow = enableCutflow,
                              enableKinematicHistograms = enableKinematicHistograms )

    # Return the sequence:
    return seq




def makeTauCalibrationSequence( seq, dataType,
                                 postfix = '',
                             rerunTruthMatching = True):
    """Create tau calibration analysis algorithms

    This makes all the algorithms that need to be run first befor
    all working point specific algorithms and that can be shared
    between the working points.

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      rerunTruthMatching -- Whether or not to rerun truth matching
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Set up the tau truth matching algorithm:
    if rerunTruthMatching and dataType != 'data':
        alg = createAlgorithm( 'CP::TauTruthMatchingAlg',
                               'TauTruthMatchingAlg' + postfix )
        addPrivateTool( alg, 'matchingTool',
                        'TauAnalysisTools::TauTruthMatchingTool' )
        alg.matchingTool.WriteTruthTaus = 1
        seq.append( alg, inputPropName = 'taus',
                    stageName = 'selection',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )
        pass

    # Set up the tau 4-momentum smearing algorithm:
    alg = createAlgorithm( 'CP::TauSmearingAlg', 'TauSmearingAlg' + postfix )
    addPrivateTool( alg, 'smearingTool', 'TauAnalysisTools::TauSmearingTool' )
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                stageName = 'calibration',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    pass





def makeTauWorkingPointSequence( seq, dataType, workingPoint, postfix = '',
                             legacyRecommendations = False):
    """Create tau analysis algorithms for a single working point

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      legacyRecommendations -- use legacy tau BDT and electron veto recommendations
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 1 :
        raise ValueError ('working point should be of format "quality", not ' + workingPoint)

    nameFormat = 'TauAnalysisAlgorithms/tau_selection_{}.conf'
    if legacyRecommendations:
        nameFormat = 'TauAnalysisAlgorithms/tau_selection_{}_legacy.conf'

    if splitWP[0] not in ['Tight', 'Medium', 'Loose', 'VeryLoose', 'NoID', 'Baseline'] :
        raise ValueError ("invalid tau quality: \"" + splitWP[0] +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose, NoID, Baseline")
    inputfile = nameFormat.format(splitWP[0].lower())

    # Setup the tau selection tool
    selectionTool = createPublicTool( 'TauAnalysisTools::TauSelectionTool',
                                      'TauSelectionTool' + postfix)
    selectionTool.ConfigPath = inputfile
    seq.addPublicTool( selectionTool, stageName = 'selection' )

    # Set up the algorithm selecting taus:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'TauSelectionAlg' + postfix )
    alg.selectionTool = '%s/%s' % \
        ( selectionTool.getType(), selectionTool.getName() )
    alg.selectionDecoration = 'selected_tau' + postfix + ',as_bits'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [6]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    # Set up the algorithm calculating the efficiency scale factors for the
    # taus:
    if dataType != 'data':
        alg = createAlgorithm( 'CP::TauEfficiencyCorrectionsAlg',
                               'TauEfficiencyCorrectionsAlg' + postfix )
        addPrivateTool( alg, 'efficiencyCorrectionsTool',
                        'TauAnalysisTools::TauEfficiencyCorrectionsTool' )
        alg.efficiencyCorrectionsTool.TauSelectionTool = '%s/%s' % \
            ( selectionTool.getType(), selectionTool.getName() )
        alg.scaleFactorDecoration = 'tau_effSF' + postfix + '_%SYS%'
        alg.outOfValidity = 2 #silent
        alg.outOfValidityDeco = 'bad_eff' + postfix
        seq.append( alg, inputPropName = 'taus',
                    stageName = 'efficiency',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    # Set up an algorithm used for decorating baseline tau selection:
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'TauSelectionSummary' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
    alg.selectionDecoration = 'baselineSelection' + postfix + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection',
                dynConfig = {'selectionTool.selectionFlags' : lambda meta : meta["selectionDecorNames"][:]} )

    pass
