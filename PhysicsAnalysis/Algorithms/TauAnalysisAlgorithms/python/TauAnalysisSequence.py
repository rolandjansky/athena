# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool, \
                                       createPublicTool

def makeTauAnalysisSequence( dataType ):
    """Create a tau analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "TauAnalysisSequence" )

    # Set up the tau 4-momentum smearing algorithm:
    alg = createAlgorithm( 'CP::TauSmearingAlg', 'TauSmearingAlg' )
    addPrivateTool( alg, 'smearingTool', 'TauAnalysisTools::TauSmearingTool' )
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                affectingSystematics = '(^TAUS_TRUEHADTAU_SME_TES_.*)' )

    # Set up the algorithm selecting "good" taus:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'TauSelectionAlg' )
    addPrivateTool( alg, 'selectionTool', 'TauAnalysisTools::TauSelectionTool' )
    alg.selectionDecoration = 'good_tau'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    # Set up the algorithm calculating the efficiency scale factors for the
    # taus:
    alg = createAlgorithm( 'CP::TauEfficiencyCorrectionsAlg',
                           'TauEfficiencyCorrectionsAlg' )
    addPrivateTool( alg, 'efficiencyCorrectionsTool',
                    'TauAnalysisTools::TauEfficiencyCorrectionsTool' )
    selTool = createPublicTool( 'TauSelectionTool',
                                'TauAnalysisTools::TauSelectionTool' )
    alg.efficiencyCorrectionsTool.TauSelectionTool = '%s/%s' % \
        ( selTool.getType(), selTool.getName() )
    seq.addPublicTool( selTool )
    alg.efficiencyDecoration = 'tau_eff'
    # alg.outOfValidity = 2 #silent
    # alg.outOfValidityDeco = "bad_eff"
    seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut',
                affectingSystematics = '(^TAUS_TRUEELECTRON_EFF_.*)|(^TAUS_TRUEHADTAU_EFF_.*)' )

    # Set up the tau truth matching algorithm:
    if dataType != 'data':
        alg = createAlgorithm( 'CP::TauTruthMatchingAlg',
                               'TauTruthMatchingAlg' )
        addPrivateTool( alg, 'matchingTool',
                        'TauAnalysisTools::TauTruthMatchingTool' )
        alg.matchingTool.WriteTruthTaus = 1
        seq.append( alg, inputPropName = 'taus', outputPropName = 'tausOut' )
        pass

    # Set up an algorithm used for debugging the tau selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'TauCutFlowDumperAlg' )
    alg.histPattern = 'tau_cflow_%SYS%'
    alg.selection = [ 'good_tau' ]
    alg.selectionNCuts = [ 6 ]
    seq.append( alg, inputPropName = 'input' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'TauViewFromSelectionAlg' )
    alg.selection = [ 'good_tau' ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up an algorithm dumping the properties of the taus, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'TauKinematicDumperAlg' )
    alg.histPattern = "tau_%VAR%_%SYS%"
    seq.append( alg, inputPropName = 'input' )

    # Return the sequence:
    return seq
