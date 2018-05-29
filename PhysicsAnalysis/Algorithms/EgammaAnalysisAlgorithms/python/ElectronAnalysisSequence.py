# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeElectronAnalysisSequence( dataType,
                                  isolationWP = 'GradientLoose',
                                  likelihoodWP = 'LooseLHElectron' ):
    """Create an electron analysis algorithm sequence

    Keywrod arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      isolationWP -- The isolation selection working point to use
      likelihoodWP -- The likelihood selection working point to use
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "ElectronAnalysisSequence" )

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

    # Set up the likelihood ID selection algorithm:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronLikelihoodAlg' )
    addPrivateTool( alg, 'selectionTool', 'AsgElectronLikelihoodTool' )
    alg.selectionTool.primaryVertexContainer = 'PrimaryVertices'
    alg.selectionTool.WorkingPoint = likelihoodWP
    alg.selectionDecoration = 'selectLikelihood'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

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
