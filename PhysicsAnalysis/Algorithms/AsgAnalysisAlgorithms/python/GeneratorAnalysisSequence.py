# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeGeneratorAnalysisSequence( dataType,
                                   saveCutBookkeepers=False,
                                   runNumber=0,
                                   cutBookkeepersSystematics=False ):
    """Create a generator analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("mc" or "afii")
      saveCutBookkeepers -- save cut bokkeepers information into output file
      runNumber -- MC run number
      cutBookkeepersSystematics -- store CutBookkeepers systematics
    """

    if dataType not in ["mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if saveCutBookkeepers and not runNumber:
        raise ValueError ("invalid run number: " + 0)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "GeneratorAnalysisSequence" )

    # Set up the CutBookkeepers algorithm:
    if saveCutBookkeepers:
      alg = createAlgorithm('CP::AsgCutBookkeeperAlg', 'CutBookkeeperAlg')
      alg.runNumber = runNumber
      alg.enableSystematics = cutBookkeepersSystematics
      addPrivateTool( alg, 'truthWeightTool', 'PMGTools::PMGTruthWeightTool' )
      seq.append( alg, inputPropName = None )

    # Set up the weights algorithm:
    alg = createAlgorithm( 'CP::PMGTruthWeightAlg', 'PMGTruthWeightAlg' )
    addPrivateTool( alg, 'truthWeightTool', 'PMGTools::PMGTruthWeightTool' )
    alg.decoration = 'generatorWeight_%SYS%'
    alg.decorationRegex = '(^GEN_.*)'

    seq.append( alg, inputPropName = 'eventInfo',
                affectingSystematics = '(^GEN_.*)' )

    # Return the sequence:
    return seq
