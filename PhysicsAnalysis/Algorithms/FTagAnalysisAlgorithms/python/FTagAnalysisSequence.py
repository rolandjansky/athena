# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeFTagAnalysisSequence( seq, dataType, jetCollection,
                              btagWP = "FixedCutBEff_77",
                              btagger = "MV2c10",
                              postfix = "",
                              preselection=None,
                              kinematicSelection = False,
                              noEfficiency = False ):
    """Create a ftag analysis algorithm sequence

    for now the sequence is passed in, I'm unsure if I can concatenate
    two sequences at the moment, or if that blows things up horribly

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      jetCollection -- Jet container to run on
      btagWP -- Flavour tagging working point
      btagger -- Flavour tagger
      kinematicSelection -- Wether to run kinematic selection
      noEfficiency -- Wether to run efficiencies calculation
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Remove b-tagging calibration from the container name
    btIndex = jetCollection.find('_BTagging')
    if btIndex != -1:
        jetCollection = jetCollection[:btIndex]

    bTagCalibFile = "xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2019-07-30_v1.root"

    # # Create the analysis algorithm sequence object:
    # seq = AnaAlgSequence( "FTagAnalysisSequence" )

    if kinematicSelection:
        # Set up the ftag kinematic selection algorithm(s):
        alg = createAlgorithm( 'CP::AsgSelectionAlg', 'FTagKinSelectionAlg'+postfix )
        addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
        alg.selectionTool.minPt = 20e3
        alg.selectionTool.maxEta = 2.5
        alg.selectionDecoration = 'ftag_kin_select'
        seq.append( alg, inputPropName = 'particles',
                    outputPropName = 'particlesOut' )

        # Set up an algorithm that makes a view container using the selections
        # performed previously:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'FTagKinViewFromSelectionAlg'+postfix )
        alg.selection = [ 'ftag_kin_select' ]
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )

    # Set up the ftag selection algorithm(s):
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'FTagSelectionAlg' + btagger + btagWP + postfix )
    addPrivateTool( alg, 'selectionTool', 'BTaggingSelectionTool' )
    alg.selectionTool.TaggerName = btagger
    alg.selectionTool.OperatingPoint = btagWP
    alg.selectionTool.JetAuthor = jetCollection
    alg.selectionTool.FlvTagCutDefinitionsFileName = bTagCalibFile
    if preselection is not None:
        alg.preselection = preselection
    alg.selectionDecoration = 'ftag_select_' + btagger + '_' + btagWP + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection' )

    if not noEfficiency and dataType != 'data':
        # Set up the efficiency calculation algorithm:
        alg = createAlgorithm( 'CP::BTaggingEfficiencyAlg',
                               'FTagEfficiencyScaleFactorAlg' + btagger + btagWP + postfix )
        addPrivateTool( alg, 'efficiencyTool',
                        'BTaggingEfficiencyTool' )
        alg.efficiencyTool.TaggerName = btagger
        alg.efficiencyTool.OperatingPoint = btagWP
        alg.efficiencyTool.JetAuthor = jetCollection
        alg.efficiencyTool.ScaleFactorFileName = bTagCalibFile
        alg.efficiencyTool.SystematicsStrategy = "Envelope"
        alg.scaleFactorDecoration = 'ftag_effSF_' + btagger + '_' + btagWP + '_%SYS%'
        alg.scaleFactorDecorationRegex = '(^FT_EFF_.*)'
        alg.selectionDecoration = 'ftag_select_' + btagger + '_' + btagWP + ',as_char'
        alg.outOfValidity = 2
        alg.outOfValidityDeco = 'no_ftag_' + btagger + '_' + btagWP
        if preselection is not None:
            alg.preselection = preselection
        seq.append( alg, inputPropName = 'jets',
                    affectingSystematics = '(^FT_EFF_.*)',
                    stageName = 'efficiency' )
        pass

    # Set up an algorithm used for debugging the f-tag selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'FTagCutFlowDumperAlg' + btagger + btagWP + postfix )
    alg.histPattern = 'ftag_cflow_' + btagger + '_' + btagWP + '_%SYS%'
    alg.selection = ['ftag_select_' + btagger + '_' + btagWP + ',as_char']
    alg.selectionNCuts = [1] # really we have 4 cuts, but we use char
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Return the sequence:
    return seq
