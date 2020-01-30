# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import addPrivateTool, createAlgorithm

def makeSequence (dataType) :
    algSeq = AlgSequence()

    # Set up the systematics loader/handler algorithm:
    sysLoader = createAlgorithm( 'CP::SysListLoaderAlg', 'SysLoaderAlg' )
    sysLoader.sigmaRecommended = 1
    algSeq += sysLoader

    # Include, and then set up the jet analysis algorithm sequence:
    from JetAnalysisAlgorithms.JetAnalysisSequence import makeJetAnalysisSequence
    jetContainer = 'AntiKt4EMPFlowJets'
    jetSequence = makeJetAnalysisSequence( dataType, jetContainer )
    jetSequence.configure( inputName = jetContainer, outputName = 'AnalysisJets_%SYS%' )

    # Add all algorithms to the job:
    algSeq += jetSequence

    # Set up a selection alg for demonstration purposes
    # Also to avoid warnings from building MET with very soft electrons
    selalg = createAlgorithm( 'CP::AsgSelectionAlg', 'METEleSelAlg' )
    addPrivateTool( selalg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    selalg.selectionTool.minPt = 10e3
    selalg.selectionTool.maxEta = 2.47
    selalg.selectionDecoration = 'selectPtEta'
    selalg.particles = 'Electrons'
    # We need to copy here, because w/o an output container, it's assumed
    # that the input container is non-const
    selalg.particlesOut = 'DecorElectrons_%SYS%'
    algSeq += selalg

    # Now make a view container holding only the electrons for the MET calculation
    viewalg = createAlgorithm( 'CP::AsgViewFromSelectionAlg','METEleViewAlg' )
    viewalg.selection = [ 'selectPtEta' ]
    viewalg.input = 'DecorElectrons_%SYS%'
    viewalg.output = 'METElectrons_%SYS%'
    algSeq += viewalg

    # Include, and then set up the met analysis algorithm sequence:
    from MetAnalysisAlgorithms.MetAnalysisSequence import makeMetAnalysisSequence
    metSequence = makeMetAnalysisSequence( dataType, metSuffix = jetContainer[:-4] )
    metSequence.configure( inputName = { 'jets'      : 'AnalysisJets_%SYS%',
                                         'muons'     : 'Muons',
                                         'electrons' : 'METElectrons_%SYS%' },
                           outputName = 'AnalysisMET_%SYS%',
                           affectingSystematics = { 'jets'      : jetSequence.affectingSystematics(),
                                                    'muons'     : '(^$)',
                                                    'electrons' : '(^$)' } )

    # Add the sequence to the job:
    algSeq += metSequence

    # Write the freshly produced MET object(s) to an output file:
    treeMaker = createAlgorithm( 'CP::TreeMakerAlg', 'TreeMaker' )
    treeMaker.TreeName = 'met'
    algSeq += treeMaker
    ntupleMaker = createAlgorithm( 'CP::AsgxAODNTupleMakerAlg', 'NTupleMaker' )
    ntupleMaker.TreeName = 'met'
    ntupleMaker.Branches = [ 'EventInfo.runNumber     -> runNumber',
                             'EventInfo.eventNumber   -> eventNumber',
                             'AnalysisMET_%SYS%.mpx   -> met_%SYS%_mpx',
                             'AnalysisMET_%SYS%.mpy   -> met_%SYS%_mpy',
                             'AnalysisMET_%SYS%.sumet -> met_%SYS%_sumet',
                             'AnalysisMET_%SYS%.name  -> met_%SYS%_name', ]
    ntupleMaker.systematicsRegex = '.*'
    algSeq += ntupleMaker
    treeFiller = createAlgorithm( 'CP::TreeFillerAlg', 'TreeFiller' )
    treeFiller.TreeName = 'met'
    algSeq += treeFiller

    return algSeq
