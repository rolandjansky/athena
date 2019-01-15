# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Framework import(s):
import ROOT

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeElectronAnalysisSequence( dataType, workingPoint,
                                  deepCopyOutput = False,
                                  postfix = '',
                                  recomputeLikelihood = False,
                                  chargeIDSelection = False,
                                  isolationCorrection = False ):
    """Create an electron analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      recomputeLikelihood -- Whether to rerun the LH. If not, use derivation flags
      chargeIDSelection -- Whether or not to perform charge ID/flip selection
      isolationCorrection -- Whether or not to perform isolation correction
    """

    # Make sure we received a valid data type.
    if not dataType in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %' % dataType )

    if postfix != '' :
        postfix = '_' + postfix
        pass

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 2 :
        raise ValueError ('working point should be of format "likelihood.isolation", not ' + workingPoint)

    likelihoodWP = splitWP[0]
    isolationWP = splitWP[1]

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "ElectronAnalysisSequence" + postfix )

    # Variables keeping track of the selections being applied.
    selectionDecorNames = []
    selectionDecorCount = []

    # Set up the an eta-cut on all electrons prior to everything else
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronEtaCutAlg' + postfix )
    alg.selectionDecoration = 'selectEta'
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.maxEta = 2.47
    alg.selectionTool.useClusterEta = True
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'calibration' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 2 )

    # Set up the likelihood ID selection algorithm
    # It is safe to do this before calibration, as the cluster E is used
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronLikelihoodAlg' + postfix )
    alg.selectionDecoration = 'selectLikelihood' + postfix
    selectionDecorNames.append( alg.selectionDecoration )
    if recomputeLikelihood:
        # Rerun the likelihood ID
        addPrivateTool( alg, 'selectionTool', 'AsgElectronLikelihoodTool' )
        alg.selectionTool.primaryVertexContainer = 'PrimaryVertices'
        alg.selectionTool.WorkingPoint = likelihoodWP
        selectionDecorCount.append( 7 )
    else:
        # Select from Derivation Framework flags
        addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
        dfFlag = "DFCommonElectronsLH" + likelihoodWP.split('LH')[0]
        alg.selectionTool.selectionFlags = [dfFlag]
        selectionDecorCount.append( 1 )
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection' )

    # Select electrons only with good object quality.
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronObjectQualityAlg' + postfix )
    alg.selectionDecoration = 'goodOQ'
    addPrivateTool( alg, 'selectionTool', 'CP::EgammaIsGoodOQSelectionTool' )
    alg.selectionTool.Mask = ROOT.xAOD.EgammaParameters.BADCLUSELECTRON
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'calibration' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 1 )

    # Only run subsequent processing on the objects passing all of these cuts.
    # Since these are independent of the electron calibration, and this speeds
    # up the job.
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'ElectronPreSelViewFromSelectionAlg' + postfix )
    alg.selection = selectionDecorNames[ : ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                stageName = 'selection' )

    # Set up the calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg',
                           'ElectronCalibrationAndSmearingAlg' + postfix )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::EgammaCalibrationAndSmearingTool' )
    alg.calibrationAndSmearingTool.ESModel = 'es2017_R21_v1'
    alg.calibrationAndSmearingTool.decorrelationModel = '1NP_v1'
    if dataType == 'afii':
        alg.calibrationAndSmearingTool.useAFII = 1
    else:
        alg.calibrationAndSmearingTool.useAFII = 0
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                affectingSystematics = '(^EG_RESOLUTION_.*)|(^EG_SCALE_.*)',
                stageName = 'calibration' )

    # Set up the isolation correction algorithm:
    if isolationCorrection:
        alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg',
                               'ElectronIsolationCorrectionAlg' + postfix )
        addPrivateTool( alg, 'isolationCorrectionTool',
                        'CP::IsolationCorrectionTool' )
        if dataType == 'data':
            alg.isolationCorrectionTool.IsMC = 0
        else:
            alg.isolationCorrectionTool.IsMC = 1
            pass
        seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                    stageName = 'calibration' )

    # Set up the isolation selection algorithm:
    if isolationWP != 'NonIso' :
        alg = createAlgorithm( 'CP::EgammaIsolationSelectionAlg',
                               'ElectronIsolationSelectionAlg' + postfix )
        alg.selectionDecoration = 'isolated' + postfix
        addPrivateTool( alg, 'selectionTool', 'CP::IsolationSelectionTool' )
        alg.selectionTool.ElectronWP = isolationWP
        seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                    stageName = 'selection' )
        selectionDecorNames.append( alg.selectionDecoration )
        selectionDecorCount.append( 1 )

    # Set up the track selection algorithm:
    alg = createAlgorithm( 'CP::AsgLeptonTrackSelectionAlg',
                           'ElectronTrackSelectionAlg' + postfix )
    alg.selectionDecoration = 'trackSelection' + postfix
    alg.maxD0Significance = 5
    alg.maxDeltaZ0SinTheta = 0.5
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 3 )

    # Select electrons only if they don't appear to have flipped their charge.
    if chargeIDSelection:
        alg = createAlgorithm( 'CP::AsgSelectionAlg',
                               'ElectronChargeIDSelectionAlg' + postfix )
        alg.selectionDecoration = 'chargeID' + postfix
        addPrivateTool( alg, 'selectionTool',
                        'AsgElectronChargeIDSelectorTool' )
        alg.selectionTool.TrainingFile = \
          'ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root'
        alg.selectionTool.WorkingPoint = 'Loose'
        alg.selectionTool.CutOnBDT = -0.337671 # Loose 97%
        seq.append( alg, inputPropName = 'particles',
                    outputPropName = 'particlesOut',
                    stageName = 'selection' )
        selectionDecorNames.append( alg.selectionDecoration )
        selectionDecorCount.append( 1 )
        pass

    # Set up the electron efficiency correction algorithm:
    alg = createAlgorithm( 'CP::ElectronEfficiencyCorrectionAlg',
                           'ElectronEfficiencyCorrectionAlg' + postfix )
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgElectronEfficiencyCorrectionTool' )
    alg.efficiencyCorrectionTool.RecoKey = "Reconstruction"
    alg.efficiencyCorrectionTool.CorrelationModel = "TOTAL"
    alg.efficiencyDecoration = 'effCor' + postfix
    if dataType == 'afii':
        alg.efficiencyCorrectionTool.ForceDataType = \
          ROOT.PATCore.ParticleDataType.Fast
    elif dataType == 'mc':
        alg.efficiencyCorrectionTool.ForceDataType = \
          ROOT.PATCore.ParticleDataType.Full
        pass
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff' + postfix
    if dataType != 'data':
        seq.append( alg, inputPropName = 'electrons',
                    outputPropName = 'electronsOut',
                    affectingSystematics = '(^EL_EFF_.*)',
                    stageName = 'efficiency' )
        selectionDecorNames.append( alg.outOfValidityDeco )
        selectionDecorCount.append( 1 )
        pass

    # Set up an algorithm used for debugging the electron selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                           'ElectronCutFlowDumperAlg' + postfix )
    alg.histPattern = 'electron_cflow_%SYS%' + postfix
    alg.selection = selectionDecorNames[ : ]
    alg.selectionNCuts = selectionDecorCount[ : ]
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'ElectronViewFromSelectionAlg' + postfix )
    alg.selection = selectionDecorNames[ : ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                stageName = 'selection' )

    # Set up an algorithm dumping the properties of the electrons, for
    # debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg',
                           'ElectronKinematicDumperAlg' + postfix )
    alg.histPattern = 'electron_%VAR%_%SYS%' + postfix
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'ElectronDeepCopyMaker' + postfix )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Return the sequence:
    return seq
