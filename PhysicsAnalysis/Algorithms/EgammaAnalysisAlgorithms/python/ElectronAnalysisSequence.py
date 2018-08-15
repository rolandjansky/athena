# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeElectronAnalysisSequence( dataType,
                                  isolationWP = 'GradientLoose',
                                  likelihoodWP = 'LooseLHElectron',
                                  recomputeLikelihood = False,
                                  prefilterLikelihood = False ):
    """Create an electron analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      isolationWP -- The isolation selection working point to use
      likelihoodWP -- The likelihood selection working point to use
      recomputeLikelihood -- Whether to rerun the LH. If not, use derivation flags
      prefilterLikelihood -- Creates intermediate selection on LH, in case of cluster thinning etc
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "ElectronAnalysisSequence" )

    # Set up the an eta-cut on all electrons prior to everything else
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronEtaCutAlg' )
    alg.selectionDecoration = 'selectEta'
    etaNcuts = 2
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.maxEta = 2.47
    alg.selectionTool.useClusterEta = True
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    # Set up the likelihood ID selection algorithm
    # It is safe to do this before calibration, as the cluster E is used
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronLikelihoodAlg' )
    alg.selectionDecoration = 'selectLikelihood'
    lhNcuts = 1
    if recomputeLikelihood:
        # Rerun the likelihood ID
        addPrivateTool( alg, 'selectionTool', 'AsgElectronLikelihoodTool' )
        alg.selectionTool.primaryVertexContainer = 'PrimaryVertices'
        alg.selectionTool.WorkingPoint = likelihoodWP
        lhNcuts = 7
    else:
        # Select from Derivation Framework flags
        addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
        dfFlag = "DFCommonElectronsLH" + likelihoodWP.split('LH')[0]
        alg.selectionTool.selectionFlags = [dfFlag]
        lhNcuts = 1
    seq.append( alg, inputPropName = 'particles',
    outputPropName = 'particlesOut' )

    # Only run subsequent processing on the objects passing eta and LH cut
    # The later is needed e.g. for top derivations that thin the clusters
    # from the electrons failing Loose(LH)
    # Basically invalidates the first cutflow step
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'ElectronLHViewFromSelectionAlg' )
    if prefilterLikelihood:
        alg.selection = [ 'selectEta', 'selectLikelihood' ]
        pass
    else :
        alg.selection = [ 'selectEta' ]
        pass
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up the calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg',
                           'ElectronCalibrationAndSmearingAlg' )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::EgammaCalibrationAndSmearingTool' )
    alg.calibrationAndSmearingTool.ESModel = 'es2017_R21_PRE'
    alg.calibrationAndSmearingTool.decorrelationModel = '1NP_v1'
    if dataType == 'afii':
        alg.calibrationAndSmearingTool.useAFII = 1
    else:
        alg.calibrationAndSmearingTool.useAFII = 0
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                affectingSystematics = '(^EG_RESOLUTION_.*)|(^EG_SCALE_.*)' )

    # Set up the isolation correction algorithm:
    alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg',
                           'ElectronIsolationCorrectionAlg' )
    addPrivateTool( alg, 'isolationCorrectionTool',
                    'CP::IsolationCorrectionTool' )
    if dataType == 'data':
        alg.isolationCorrectionTool.IsMC = 0
    else:
        alg.isolationCorrectionTool.IsMC = 1
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut' )

    # Set up the isolation selection algorithm:
    alg = createAlgorithm( 'CP::EgammaIsolationSelectionAlg',
                           'ElectronIsolationSelectionAlg' )
    addPrivateTool( alg, 'selectionTool', 'CP::IsolationSelectionTool' )
    alg.selectionTool.ElectronWP = isolationWP
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut' )

    # Set up the electron efficiency correction algorithm:
    alg = createAlgorithm( 'CP::ElectronEfficiencyCorrectionAlg',
                           'ElectronEfficiencyCorrectionAlg' )
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgElectronEfficiencyCorrectionTool' )
    alg.efficiencyCorrectionTool.MapFilePath = \
        'ElectronEfficiencyCorrection/2015_2017/rel21.2/Summer2017_Prerec_v1/map0.txt'
    alg.efficiencyCorrectionTool.RecoKey = "Reconstruction"
    alg.efficiencyCorrectionTool.CorrelationModel = "TOTAL"
    alg.efficiencyCorrectionTool.CorrelationModel = "TOTAL"
    alg.efficiencyDecoration = 'effCor'
    if dataType == 'afii':
        alg.efficiencyCorrectionTool.ForceDataType = 3
    else :
        alg.efficiencyCorrectionTool.ForceDataType = 1
        pass
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff'
    seq.append( alg, inputPropName = 'electrons',
                outputPropName = 'electronsOut',
                affectingSystematics = '(^EL_EFF_.*)' )

    # Set up an algorithm used for debugging the electron selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                           'ElectronCutFlowDumperAlg' )
    alg.histPattern = 'electron_cflow_%SYS%'
    alg.selection = [ 'selectLikelihood', 'isolated', 'bad_eff' ]
    alg.selectionNCuts = [ 7, 1, 1 ]
    seq.append( alg, inputPropName = 'input' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'ElectronViewFromSelectionAlg' )
    alg.selection = [ 'selectLikelihood', 'isolated', 'bad_eff' ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up an algorithm dumping the properties of the electrons, for
    # debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg',
                           'ElectronKinematicDumperAlg' )
    alg.histPattern = 'electron_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input' )

    # Return the sequence:
    return seq
