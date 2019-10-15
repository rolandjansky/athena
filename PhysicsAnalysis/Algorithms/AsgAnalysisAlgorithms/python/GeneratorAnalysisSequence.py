# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeGeneratorAnalysisSequence( dataType ):
    """Create a generator analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("mc" or "afii")
    """

    if not dataType in ["mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "GeneratorAnalysisSequence" )

    # Set up the weights algorithm:
    alg = createAlgorithm( 'CP::PMGTruthWeightAlg', 'PMGTruthWeightAlg' )
    addPrivateTool( alg, 'truthWeightTool', 'PMGTools::PMGTruthWeightTool' )
    alg.decoration = 'generatorWeight_%SYS%'
    alg.decorationRegex = '(^GEN_.*)'

    seq.append( alg, inputPropName = 'eventInfo',
                affectingSystematics = '(^GEN_.*)' )

    # Return the sequence:
    return seq
