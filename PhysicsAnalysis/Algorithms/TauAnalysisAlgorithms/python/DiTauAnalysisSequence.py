# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AsgAnalysisAlgorithms.AnalysisObjectSharedSequence import makeSharedObjectSequence
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeDiTauAnalysisSequence( dataType, workingPoint, postfix = '',
                             deepCopyOutput = False,
                             shallowViewOutput = True,
                             rerunTruthMatching = True,
                             enableCutflow = False,
                             enableKinematicHistograms = False ):
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
    seq = AnaAlgSequence( "DiTauAnalysisSequence" + postfix )

    seq.addMetaConfigDefault ("selectionDecorNames", [])
    seq.addMetaConfigDefault ("selectionDecorNamesOutput", [])
    seq.addMetaConfigDefault ("selectionDecorCount", [])

    makeDiTauCalibrationSequence (seq, dataType, postfix=postfix,
                                rerunTruthMatching = rerunTruthMatching)
    makeDiTauWorkingPointSequence (seq, dataType, workingPoint, postfix=postfix)
    makeSharedObjectSequence (seq, deepCopyOutput = deepCopyOutput,
                              shallowViewOutput = shallowViewOutput,
                              postfix = '_DiTau' + postfix,
                              enableCutflow = enableCutflow,
                              enableKinematicHistograms = enableKinematicHistograms )

    # Return the sequence:
    return seq




def makeDiTauCalibrationSequence( seq, dataType,
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

    # Set up the tau 4-momentum smearing algorithm:
    alg = createAlgorithm( 'CP::DiTauSmearingAlg', 'DiTauSmearingAlg' + postfix )
    addPrivateTool( alg, 'smearingTool', 'TauAnalysisTools::DiTauSmearingTool' )
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                stageName = 'calibration',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    # Set up the tau truth matching algorithm:
    if dataType != 'data':
        alg = createAlgorithm( 'CP::DiTauTruthMatchingAlg',
                               'DiTauTruthMatchingAlg' + postfix )
        addPrivateTool( alg, 'matchingTool',
                        'TauAnalysisTools::DiTauTruthMatchingTool' )
        alg.matchingTool.WriteTruthTaus = 1
        seq.append( alg, inputPropName = 'taus',
                    stageName = 'selection',
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )
        pass

    pass





def makeDiTauWorkingPointSequence( seq, dataType, workingPoint, postfix = '',
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

    # using enum value from: https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/TauAnalysisTools/Enums.h
    # the dictionary is missing in Athena, so hard-coding values here
    if splitWP[0] == 'Tight' :
        IDLevel = 4 # ROOT.TauAnalysisTools.JETIDBDTTIGHT
        pass
    elif splitWP[0] == 'Medium' :
        IDLevel = 3 # ROOT.TauAnalysisTools.JETIDBDTMEDIUM
        pass
    elif splitWP[0] == 'Loose' :
        IDLevel = 2 # ROOT.TauAnalysisTools.JETIDBDTLOOSE
        pass
    else :
        raise ValueError ("invalid tau quality: \"" + splitWP[0] +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose")

    # Set up the algorithm calculating the efficiency scale factors for the
    # taus:
    alg = createAlgorithm( 'CP::DiTauEfficiencyCorrectionsAlg',
                           'DiTauEfficiencyCorrectionsAlg' + postfix )
    addPrivateTool( alg, 'efficiencyCorrectionsTool',
                    'TauAnalysisTools::DiTauEfficiencyCorrectionsTool' )
    alg.efficiencyCorrectionsTool.IDLevel = IDLevel
    alg.scaleFactorDecoration = 'tau_effSF' + postfix
    # alg.outOfValidity = 2 #silent
    # alg.outOfValidityDeco = "bad_eff"
    seq.append( alg, inputPropName = 'taus',
                stageName = 'efficiency',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    pass
