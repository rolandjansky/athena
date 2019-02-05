# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, createPublicTool


def makeTriggerAnalysisSequence( dataType,
                                 triggerChains = []):
    """Create a basic trigger analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      triggerChains -- a list of trigger chains
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "TriggerAnalysisSequence" )

    # Create public trigger tools
    xAODConfTool = createPublicTool( 'TrigConf::xAODConfigTool', 'xAODConfigTool' )
    decisionTool = createPublicTool( 'Trig::TrigDecisionTool', 'TrigDecisionTool')
    decisionTool.ConfigTool = '%s/%s' % \
        ( xAODConfTool.getType(), xAODConfTool.getName() )

    seq.addPublicTool( xAODConfTool )
    seq.addPublicTool( decisionTool )

    # Set up the trigger selection:
    if triggerChains:
        alg = createAlgorithm( 'CP::TrigEventSelectionAlg', 'TrigEventSelectorAlg' )
        alg.tool = '%s/%s' % \
            ( decisionTool.getType(), decisionTool.getName() )
        alg.triggers = list(triggerChains)
        alg.selectionDecoration = 'trigPassed'

        seq.append( alg, inputPropName = None )

    # Return the sequence:
    return seq
