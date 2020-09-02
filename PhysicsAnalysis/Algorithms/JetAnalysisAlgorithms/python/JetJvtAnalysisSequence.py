# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

def makeJetJvtAnalysisSequence( dataType, jetCollection,
                                preselection = '',
                                disableFJvt = False,
                                globalSF = True,
                                runSelection = True,
                                enableCutflow = False ):
    """Create a jet JVT analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      jetCollection -- The jet container to run on
      disableFJvt -- Whether to disable forward JVT calculations
      globalSF -- Whether to calculate per event scale factors
      runSelection -- Whether to run selection
      enableCutflow -- Whether or not to dump the cutflow
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if runSelection and not globalSF :
        raise ValueError ("per-event scale factors needs to be computed when doing a JVT selection")

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "JetJVTAnalysisSequence" )

    # Define a list of cuts to apply later on and the
    # number of bits in the corresponding TAccept
    cutlist = []
    cutlength = []

    # Set up the per-event jet efficiency scale factor calculation algorithm
    if dataType != 'data' and globalSF:
        from JetAnalysisSequence import jvtSysts, fjvtSysts

        alg = createAlgorithm( 'CP::AsgEventScaleFactorAlg', 'JvtEventScaleFactorAlg' )
        alg.preselection = preselection + '&&no_jvt' if preselection else 'no_jvt'
        alg.scaleFactorInputDecoration = 'jvt_effSF_%SYS%'
        alg.scaleFactorInputDecorationRegex = jvtSysts
        alg.scaleFactorOutputDecoration = 'jvt_effSF_%SYS%'

        seq.append( alg,
                    affectingSystematics = jvtSysts,
                    inputPropName = { 'jets' : 'particles',
                                      'eventInfo' : 'eventInfo' } )

        if not disableFJvt:
            alg = createAlgorithm( 'CP::AsgEventScaleFactorAlg', 'ForwardJvtEventScaleFactorAlg' )
            alg.preselection = preselection + '&&no_fjvt' if preselection else 'no_fjvt'
            alg.scaleFactorInputDecoration = 'fjvt_effSF_%SYS%'
            alg.scaleFactorInputDecorationRegex = fjvtSysts
            alg.scaleFactorOutputDecoration = 'fjvt_effSF_%SYS%'

            seq.append( alg,
                        affectingSystematics = fjvtSysts,
                        inputPropName = { 'jets' : 'particles',
                                          'eventInfo' : 'eventInfo' } )

    if runSelection:
        cutlist.append('jvt_selection')
        cutlength.append(1)

        if not disableFJvt:
            cutlist.append('fjvt_selection')
            cutlength.append(1)

        # Set up an algorithm used to create jet JVT selection cutflow:
        if enableCutflow:
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
