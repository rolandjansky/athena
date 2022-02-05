# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeSharedObjectSequence( seq,
                              deepCopyOutput = False,
                              shallowViewOutput = True,
                              postfix = '',
                              enableCutflow = False,
                              enableKinematicHistograms = False ):
    """Create an analysis algorithm sequence for shared post-processing of
    all object types

    This contains various algorithms for producing view containers (or
    deep copied containers), as well as debugging output.  Typically
    there has to be one such sequence for each object type and working
    point.  Due to the highly regular format, as well as the number of
    customizations (and all algorithms being switchable) it may be
    more practical for the users to write their own version of this
    for their use case.

    Keyword arguments:
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      shallowViewOutput -- Create a view container if required
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is required to make multiple such
                 sequences for various object types and working points.
      enableCutflow -- Whether or not to dump the cutflow
      enableKinematicHistograms -- Whether or not to dump the kinematic histograms

    """

    # Make sure selection options make sense
    if deepCopyOutput and shallowViewOutput:
        raise ValueError ("deepCopyOutput and shallowViewOutput can't both be true!")

    # Set up an algorithm used to create selection cutflow:
    if enableCutflow:
        alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'CutFlowDumperAlg' + postfix )
        alg.histPattern = 'cflow' + postfix + '_%SYS%'
        seq.append( alg, inputPropName = 'input', stageName = 'selection',
                    dynConfig = {'selection' : lambda meta : meta["selectionDecorNames"][:],
                                 'selectionNCuts' : lambda meta : meta["selectionDecorCount"][:]} )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    if shallowViewOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                            'ViewFromSelectionAlg' + postfix )
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection',
                    dynConfig = {'selection' : lambda meta : meta["selectionDecorNamesOutput"][:]} )

    # Set up an algorithm dumping the kinematic properties of the objects:
    if enableKinematicHistograms:
        alg = createAlgorithm( 'CP::KinematicHistAlg', 'KinematicDumperAlg' + postfix )
        alg.histPattern = '%VAR%' + postfix + '_%SYS%'
        seq.append( alg, inputPropName = 'input', stageName = 'selection',
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'DeepCopyMaker' + postfix )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection',
                    dynConfig = {'selection' : lambda meta : meta["selectionDecorNamesOutput"][:]} )
        pass

    pass
