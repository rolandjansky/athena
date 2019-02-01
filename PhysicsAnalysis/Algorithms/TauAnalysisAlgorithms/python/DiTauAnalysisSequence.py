# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool, \
                                       createPublicTool

def makeDiTauAnalysisSequence( dataType, workingPoint,
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
        pass
    elif splitWP[0] == 'Medium' :
        pass
    elif splitWP[0] == 'Loose' :
        pass
    elif splitWP[0] == 'VeryLoose' :
        pass
    else :
        raise ValueError ("invalid tau quality: \"" + splitWP[0] +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose")

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "DiTauAnalysisSequence" + postfix )

    # Set up the tau 4-momentum smearing algorithm:
    alg = createAlgorithm( 'CP::DiTauSmearingAlg', 'DiTauSmearingAlg' + postfix )
    addPrivateTool( alg, 'smearingTool', 'TauAnalysisTools::DiTauSmearingTool' )
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                affectingSystematics = '(^TAUS_TRUEHADDITAU_SME_TES_.*)',
                stageName = 'calibration' )

    # Set up an algorithm dumping the properties of the taus, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'DiTauKinematicDumperAlg' + postfix )
    alg.histPattern = "tau_%VAR%_%SYS%"
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'DiTauDeepCopyMaker' + postfix )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Return the sequence:
    return seq
