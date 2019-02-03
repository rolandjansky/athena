# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeJetJvtAnalysisSequence( dataType, jetCollection,
                                globalSF = True,
                                runSelection = True ):
    """Create a jet JVT analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      jetCollection -- The jet container to run on
      globalSF -- Wether to calculate per event efficiencies
      runSelection -- Wether to run selection
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if runSelection and not globalSF :
        raise ValueError ("per-event efficiency needs to be computed when doing a JVT selection")

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "JetJVTAnalysisSequence" )

    # Define a list of cuts to apply later on and the
    # number of bits in the corresponding TAccept
    cutlist = []
    cutlength = []

    # Set up the per-event jet efficiency scale factor calculation algorithm
    if dataType != 'data' and globalSF:
        alg = createAlgorithm( 'CP::AsgEventScaleFactorAlg', 'JvtEventScaleFactorAlg' )
        alg.efficiency = 'jvt_efficiency'
        alg.outOfValidity = 'no_jvt'

        seq.append( alg,
                    inputPropName = { 'jets' : 'particles',
                                      'eventInfo' : 'eventInfo' },
                    outputPropName = { 'jets' : 'particlesOut',
                                       'eventInfo' : 'eventInfoOut' } )

        alg = createAlgorithm( 'CP::AsgEventScaleFactorAlg', 'ForwardJvtEventScaleFactorAlg' )
        alg.efficiency = 'fjvt_efficiency'
        alg.outOfValidity= 'no_fjvt'

        seq.append( alg,
                    inputPropName = { 'jets' : 'particles',
                                      'eventInfo' : 'eventInfo' },
                    outputPropName = { 'jets' : 'particlesOut',
                                       'eventInfo' : 'eventInfoOut' } )


    if runSelection:
        cutlist.append('jvt_selection')
        cutlength.append(1)
        
        cutlist.append('fjvt_selection')
        cutlength.append(1)

        # Set up an algorithm used for debugging the jet selection:
        alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'JetJvtCutFlowDumperAlg' )
        alg.histPattern = 'jet_cflow_jvt_%SYS%'
        alg.selection = cutlist
        alg.selectionNCuts = cutlength
        seq.append( alg, inputPropName = { 'jets' : 'input' })

        # Set up an algorithm that makes a view container using the selections
        # performed previously:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'JetJvtViewFromSelectionAlg' )
        alg.selection = cutlist
        seq.append( alg, inputPropName = { 'jets' : 'input' },
                    outputPropName = { 'jets' : 'output' } )

    # Return the sequence:
    return seq
