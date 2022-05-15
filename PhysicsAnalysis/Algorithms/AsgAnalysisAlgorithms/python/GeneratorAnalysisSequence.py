# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeGeneratorAnalysisSequence( dataType,
                                   saveCutBookkeepers=False,
                                   runNumber=0,
                                   cutBookkeepersSystematics=False,
                                   generator="default",
                                   prodFractionWeight=False ):
    """Create a generator analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("mc" or "afii")
      saveCutBookkeepers -- save cut bokkeepers information into output file
      runNumber -- MC run number
      cutBookkeepersSystematics -- store CutBookkeepers systematics
      generator -- Generator for HF production fraction weights
      prodFractionWeight -- Calculate the HF production fraction weights
    """

    if dataType not in ["mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if saveCutBookkeepers and not runNumber:
        raise ValueError ("invalid run number: " + 0)

    if generator not in ["default", "Pythia8", "Sherpa221", "Sherpa228", "Sherpa2210", "Herwig7", "Herwig713", "Herwig721", "amc@NLO"]:
        raise ValueError ("invalid generator type: " + generator)

    # MC/MC scale factors configuration
    if generator == "default":
        DSID = "410470"
    elif generator == "Sherpa221":
        DSID = "410250"
    elif generator == "Sherpa228":
        DSID = "421152"
    elif generator == "Sherpa2210":
        DSID = "700122"
    elif generator == "Herwig7":
        DSID = "410558"
    elif generator == "Herwig713":
        DSID = "411233"
    elif generator == "Herwig721":
        DSID = "600666"
    elif generator == "amc@NLO":
        DSID = "410464"

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
    seq.append( alg, inputPropName = None )

    # Production fraction weights
    if prodFractionWeight:
        alg = createAlgorithm('CP::SysTruthWeightAlg', 'SysTruthWeightAlg')
        addPrivateTool(alg, 'sysTruthWeightTool', 'PMGTools::PMGHFProductionFractionTool')
        alg.decoration = 'prodFracWeight_%SYS%'
        alg.sysTruthWeightTool.ShowerGenerator = DSID
        seq.append( alg, inputPropName = 'eventInfo')

    # Return the sequence:
    return seq
