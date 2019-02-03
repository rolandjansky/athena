# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeFTagAnalysisSequence( seq, dataType, jetContainer,
                              btagWP = "FixedCutBEff_77",
                              btagger = "MV2c10",
                              kinematicSelection = False,
                              noEfficiency = False ):
    """Create a ftag analysis algorithm sequence

    for now the sequence is passed in, I'm unsure if I can concatenate
    two sequences at the moment, or if that blows things up horribly

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      jetContainer -- Jet container to run on
      btagWP -- Flavour tagging working point
      btagger -- Flavour tagger
      kinematicSelection -- Wether to run kinematic selection
      noEfficiency -- Wether to run efficiencies calculation
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    bTagCalibFile = "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-10-19_v1.root"

    # # Create the analysis algorithm sequence object:
    # seq = AnaAlgSequence( "FTagAnalysisSequence" )

    if kinematicSelection:
        # Set up the ftag kinematic selection algorithm(s):
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
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'FTagSelectionAlg' + btagger + btagWP )
    addPrivateTool( alg, 'selectionTool', 'BTaggingSelectionTool' )
    alg.selectionTool.TaggerName = btagger
    alg.selectionTool.OperatingPoint = btagWP
    alg.selectionTool.JetAuthor = jetContainer
    alg.selectionTool.FlvTagCutDefinitionsFileName = bTagCalibFile
    alg.selectionDecoration = 'ftag_select_' + btagger + '_' + btagWP + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    if not noEfficiency :
        # Set up the efficiency calculation algorithm:
        alg = createAlgorithm( 'CP::BTaggingEfficiencyAlg',
                               'FTagEfficiencyScaleFactorAlg' + btagger + btagWP )
        addPrivateTool( alg, 'efficiencyTool',
                        'BTaggingEfficiencyTool' )
        alg.efficiencyTool.TaggerName = btagger
        alg.efficiencyTool.OperatingPoint = btagWP
        alg.efficiencyTool.JetAuthor = jetContainer
        alg.efficiencyTool.ScaleFactorFileName = bTagCalibFile
        alg.efficiencyTool.SystematicsStrategy = "Envelope"
        alg.efficiencyDecoration = 'ftag_eff_' + btagger + '_' + btagWP
        alg.selectionDecoration = 'ftag_select_' + btagger + '_' + btagWP + ',as_char'
        alg.outOfValidity = 2
        alg.outOfValidityDeco = 'no_ftag_' + btagger + '_' + btagWP
        seq.append( alg, inputPropName = 'jets', outputPropName = 'jetsOut',
                    affectingSystematics = '(^FT_EFF_.*)' )
        pass

    # Set up an algorithm used for debugging the f-tag selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'FTagCutFlowDumperAlg' + btagger + btagWP )
    alg.histPattern = 'ftag_cflow_' + btagger + '_' + btagWP + '_%SYS%'
    alg.selection = ['ftag_select_' + btagger + '_' + btagWP + ',as_char']
    alg.selectionNCuts = [1] # really we have 4 cuts, but we use char
    seq.append( alg, inputPropName = 'input' )

    # Return the sequence:
    return seq
