# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# @author Tadej Novak

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool


def makeEventSelectionAnalysisSequence( dataType,
                                        runPrimaryVertexSelection = True,
                                        runEventCleaning = False,
                                        userGRLFiles = []):
    """Create a basic event selection analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      runPrimaryVertexSelection -- whether to run primary vertex selection
      runEventCleaning -- wether to run event cleaning
      userGRLFiles -- a list of GRL files to select data from
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "EventSelectionAnalysisSequence" )

    if dataType == 'data':
        grlFiles = userGRLFiles[:]

        # Set up the GRL selection:
        alg = createAlgorithm( 'GRLSelectorAlg', 'GRLSelectorAlg' )
        addPrivateTool( alg, 'Tool', 'GoodRunsListSelectionTool' )
        alg.Tool.GoodRunsListVec = grlFiles

        seq.append( alg, inputPropName = None )

    # Skip events with no primary vertex:
    if runPrimaryVertexSelection:
        alg = createAlgorithm( 'CP::VertexSelectionAlg',
                               'PrimaryVertexSelectorAlg' )
        alg.VertexContainer = 'PrimaryVertices'
        alg.MinVertices = 1

        seq.append( alg, inputPropName = None )

    # Set up the event cleaning selection:
    if runEventCleaning:
        alg = createAlgorithm( 'CP::EventFlagSelectionAlg', 'EventFlagSelectorAlg' )
        alg.selectionFlags = ['DFCommonJets_eventClean_LooseBad,as_char']

        seq.append( alg, inputPropName = None )

    # Return the sequence:
    return seq
