# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeMetAnalysisSequence( dataType, metSuffix, useFJVT = True ):
    """Create a met analysis algorithm sequence

    After creating the sequence object, it needs to be configured with a call
    like:

       metSequence.configure( inputName = {
                                 'jets'      : 'AntiKt4EMTopoJets_%SYS%',
                                 'electrons' : 'AnalysisElectrons_%SYS%',
                                 'photons'   : 'AnalysisPhotons_%SYS%',
                                 'muons'     : 'AnalysisMuons_%SYS%',
                                 'taus'      : 'AnalysisTaus_%STS%',
                              },
                              outputName = 'AnalysisMET_%SYS%',
                              affectingSystematics = {
                                 'jets'      : '(^$)|(^JET_.*)',
                                 'electrons' : '(^$)|(^EG_.*)|(^EL_.*)',
                                 'photons'   : '(^$)|(^EG_.*)|(^PH_.*)',
                                 'muons'     : '(^$)|(^MUON_.*)',
                                 'taus'      : '(^$)|(^TAUS_.*)',
                              } )

    Note that defining a jet container is mandatory, but all other input
    containers are optional.

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      metSuffix -- Suffix for the (core) MET objects to use from the input
                   (file)
      useFJVT -- Use FJVT decision for the calculation
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "MetAnalysisSequence" )

    # Set up the met maker algorithm:
    alg = createAlgorithm( 'CP::MetMakerAlg', 'MetMakerAlg' )
    addPrivateTool( alg, 'makerTool', 'met::METMaker' )
    alg.makerTool.DoPFlow = 'PFlow' in metSuffix
    if useFJVT:
        alg.makerTool.JetRejectionDec = 'passFJVT'
    alg.metCore = 'MET_Core_' + metSuffix
    alg.metAssociation = 'METAssoc_' + metSuffix
    seq.append( alg,
                inputPropName = { 'jets'      : 'jets',
                                  'electrons' : 'electrons',
                                  'photons'   : 'photons',
                                  'muons'     : 'muons',
                                  'taus'      : 'taus' },
                outputPropName = 'met',
                affectingSystematics = '(^MET_.*)' )

    if dataType != "data" :
        alg = createAlgorithm( 'CP::MetSystematicsAlg', 'MetSystematicsAlg' )
        addPrivateTool( alg, 'systematicsTool', 'met::METSystematicsTool' )
        seq.append( alg, inputPropName = 'met',
                    affectingSystematics = '(^MET_.*)' )
        pass

    # Set up the met builder algorithm:
    alg = createAlgorithm( 'CP::MetBuilderAlg', 'MetBuilderAlg' )
    seq.append( alg, inputPropName = 'met' )

    # Set up the met significance algorithm:
    alg = createAlgorithm( 'CP::MetSignificanceAlg', 'MetSignificanceAlg' )
    addPrivateTool( alg, 'significanceTool', 'met::METSignificance' )
    alg.significanceTool.SoftTermParam = 0
    alg.significanceTool.TreatPUJets = True
    seq.append( alg, inputPropName = 'met' )

    # Return the sequence:
    return seq
