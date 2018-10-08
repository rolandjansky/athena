# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeFTagAnalysisSequence( seq, dataType, jetContainer, btagWP = "FixedCutBEff_77", btagger = "MV2c10", noEfficiency = False ):
    """Create a ftag analysis algorithm sequence

    for now the sequence is passed in, I'm unsure if I can concatenate
    two sequences at the moment, or if that blows things up horribly

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    bTagCalibFile = "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-02-09_v1.root"

    # # Create the analysis algorithm sequence object:
    # seq = AnaAlgSequence( "FTagAnalysisSequence" )

    # Set up the ftag selection algorithm(s):
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'FTagKinSelectionAlg' )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.minPt = 20e3
    alg.selectionTool.maxEta = 2.5
    alg.selectionDecoration = 'ftag_kin_select'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'FTagKinViewFromSelectionAlg' )
    alg.selection = [ 'ftag_kin_select' ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up the ftag selection algorithm(s):
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'FTagSelectionAlg' )
    addPrivateTool( alg, 'selectionTool', 'BTaggingSelectionTool' )
    alg.selectionDecoration = 'good_ftag'
    alg.selectionTool.TaggerName = btagger
    alg.selectionTool.OperatingPoint = btagWP
    alg.selectionTool.JetAuthor = jetContainer
    alg.selectionTool.FlvTagCutDefinitionsFileName = bTagCalibFile
    alg.selectionDecoration = 'ftag_select'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    if not noEfficiency :
        # Set up the efficiency calculation algorithm:
        alg = createAlgorithm( 'CP::BTaggingEfficiencyAlg',
                               'FTagEfficiencyScaleFactorAlg' )
        addPrivateTool( alg, 'efficiencyTool',
                        'BTaggingEfficiencyTool' )
        alg.efficiencyTool.TaggerName = btagger
        alg.efficiencyTool.OperatingPoint = btagWP
        alg.efficiencyTool.JetAuthor = jetContainer
        alg.efficiencyTool.ScaleFactorFileName = bTagCalibFile
        alg.efficiencyTool.SystematicsStrategy = "Envelope"
        alg.efficiencyDecoration = 'ftag_eff'
        alg.selectionDecoration = 'ftag_select'
        seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut',
                    affectingSystematics = '(^FT_EFF_.*)' )
        pass

    # Return the sequence:
    return seq
