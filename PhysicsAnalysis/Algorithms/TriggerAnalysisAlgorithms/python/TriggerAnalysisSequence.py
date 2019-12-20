# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import addPrivateTool, createAlgorithm, createPublicTool


def makeTriggerAnalysisSequence( dataType,
                                 triggerChains = [],
                                 prescaleLumiCalcFiles = []):
    """Create a basic trigger analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      triggerChains -- a list of trigger chains
      prescaleLumiCalcFiles -- a list of lumicalc files to calculate trigger prescales
    """

    if dataType not in ["data", "mc", "afii"] :
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

    if triggerChains:
        # Set up the trigger selection:
        alg = createAlgorithm( 'CP::TrigEventSelectionAlg', 'TrigEventSelectorAlg' )
        alg.tool = '%s/%s' % \
            ( decisionTool.getType(), decisionTool.getName() )
        alg.triggers = list(triggerChains)
        alg.selectionDecoration = 'trigPassed'

        seq.append( alg, inputPropName = None )

        # Calculate trigger prescales
        if dataType == 'data' and prescaleLumiCalcFiles:
            alg = createAlgorithm( 'CP::TrigPrescalesAlg', 'TrigPrescalesAlg' )
            addPrivateTool( alg, 'pileupReweightingTool', 'CP::PileupReweightingTool' )
            alg.pileupReweightingTool.LumiCalcFiles = prescaleLumiCalcFiles
            alg.pileupReweightingTool.TrigDecisionTool = '%s/%s' % \
                ( decisionTool.getType(), decisionTool.getName() )
            alg.triggers = [lumicalc.split(':')[-1] for lumicalc in prescaleLumiCalcFiles if ':' in lumicalc]
            alg.triggersAll = list(triggerChains)
            alg.prescaleDecoration = 'prescale'

            seq.append( alg, inputPropName = None )

    # Return the sequence:
    return seq
