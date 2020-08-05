# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Framework import(s):
import ROOT

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

# E/gamma import(s).
from xAODEgamma.xAODEgammaParameters import xAOD

def makeElectronAnalysisSequence( dataType, workingPoint,
                                  deepCopyOutput = False,
                                  shallowViewOutput = True,
                                  postfix = '',
                                  recomputeLikelihood = False,
                                  chargeIDSelection = False,
                                  isolationCorrection = False,
                                  crackVeto = False,
                                  ptSelectionOutput = False,
                                  enableCutflow = False,
                                  enableKinematicHistograms = False ):
    """Create an electron analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      shallowViewOutput -- Create a view container if required
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      recomputeLikelihood -- Whether to rerun the LH. If not, use derivation flags
      chargeIDSelection -- Whether or not to perform charge ID/flip selection
      isolationCorrection -- Whether or not to perform isolation correction
      crackVeto -- Whether or not to perform eta crack veto
      ptSelectionOutput -- Whether or not to apply pt selection when creating
                           output containers.
      enableCutflow -- Whether or not to dump the cutflow
      enableKinematicHistograms -- Whether or not to dump the kinematic histograms
    """

    # Make sure we received a valid data type.
    if dataType not in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %' % dataType )

    if postfix != '' :
        postfix = '_' + postfix
        pass

    # Make sure selection options make sense
    if deepCopyOutput and shallowViewOutput:
        raise ValueError ("deepCopyOutput and shallowViewOutput can't both be true!")

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

    # Set up the eta-cut on all electrons prior to everything else
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronEtaCutAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    alg.selectionDecoration = 'selectEta' + postfix + ',as_bits'
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.maxEta = 2.47
    if crackVeto:
        alg.selectionTool.etaGapLow = 1.37
        alg.selectionTool.etaGapHigh = 1.52
    alg.selectionTool.useClusterEta = True
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'calibration' )
    selectionDecorNames.append( alg.selectionDecoration )
    if crackVeto :
        selectionDecorCount.append( 5 )
    else :
        selectionDecorCount.append( 4 )

    # Set up the track selection algorithm:
    alg = createAlgorithm( 'CP::AsgLeptonTrackSelectionAlg',
                           'ElectronTrackSelectionAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    alg.selectionDecoration = 'trackSelection' + postfix + ',as_bits'
    alg.maxD0Significance = 5
    alg.maxDeltaZ0SinTheta = 0.5
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 3 )

    # Set up the likelihood ID selection algorithm
    # It is safe to do this before calibration, as the cluster E is used
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronLikelihoodAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    alg.selectionDecoration = 'selectLikelihood' + postfix + ',as_bits'
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
                stageName = 'selection' )

    # Select electrons only with good object quality.
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronObjectQualityAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    alg.selectionDecoration = 'goodOQ' + postfix + ',as_bits'
    addPrivateTool( alg, 'selectionTool', 'CP::EgammaIsGoodOQSelectionTool' )
    alg.selectionTool.Mask = xAOD.EgammaParameters.BADCLUSELECTRON
    seq.append( alg, inputPropName = 'particles',
                stageName = 'calibration' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 1 )

    # Set up the calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg',
                           'ElectronCalibrationAndSmearingAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::EgammaCalibrationAndSmearingTool' )
    alg.calibrationAndSmearingTool.ESModel = 'es2018_R21_v0'
    alg.calibrationAndSmearingTool.decorrelationModel = '1NP_v1'
    if dataType == 'afii':
        alg.calibrationAndSmearingTool.useAFII = 1
    else:
        alg.calibrationAndSmearingTool.useAFII = 0
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                affectingSystematics = '(^EG_RESOLUTION_.*)|(^EG_SCALE_.*)',
                stageName = 'calibration' )

    # Set up the the pt selection
    ptSelectionDecoration = 'selectPt' + postfix + ',as_bits'
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronPtCutAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    alg.selectionDecoration = ptSelectionDecoration
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.minPt = 4.5e3
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 2 )

    # Set up the isolation correction algorithm:
    if isolationCorrection:
        alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg',
                               'ElectronIsolationCorrectionAlg' + postfix )
        alg.preselection = "&&".join (selectionDecorNames)
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
        alg.preselection = "&&".join (selectionDecorNames)
        alg.selectionDecoration = 'isolated' + postfix + ',as_bits'
        addPrivateTool( alg, 'selectionTool', 'CP::IsolationSelectionTool' )
        alg.selectionTool.ElectronWP = isolationWP
        seq.append( alg, inputPropName = 'egammas',
                    stageName = 'selection' )
        selectionDecorNames.append( alg.selectionDecoration )
        selectionDecorCount.append( 1 )

    # Select electrons only if they don't appear to have flipped their charge.
    if chargeIDSelection:
        alg = createAlgorithm( 'CP::AsgSelectionAlg',
                               'ElectronChargeIDSelectionAlg' + postfix )
        alg.preselection = "&&".join (selectionDecorNames)
        alg.selectionDecoration = 'chargeID' + postfix + ',as_bits'
        addPrivateTool( alg, 'selectionTool',
                        'AsgElectronChargeIDSelectorTool' )
        alg.selectionTool.TrainingFile = \
          'ElectronPhotonSelectorTools/ChargeID/ECIDS_20180731rel21Summer2018.root'
        alg.selectionTool.WorkingPoint = 'Loose'
        alg.selectionTool.CutOnBDT = -0.337671 # Loose 97%
        seq.append( alg, inputPropName = 'particles',
                    stageName = 'selection' )
        selectionDecorNames.append( alg.selectionDecoration )
        selectionDecorCount.append( 1 )
        pass

    # Set up an algorithm used for decorating baseline electron selection:
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'ElectronSelectionSummary' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
    alg.selectionTool.selectionFlags = selectionDecorNames[ : ]
    alg.selectionDecoration = 'baselineSelection' + postfix + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection' )

    # Set up an algorithm used to create electron selection cutflow:
    if enableCutflow:
        alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'ElectronCutFlowDumperAlg' + postfix )
        alg.histPattern = 'electron_cflow_%SYS%' + postfix
        alg.selection = selectionDecorNames[ : ]
        alg.selectionNCuts = selectionDecorCount[ : ]
        seq.append( alg, inputPropName = 'input', stageName = 'selection' )

    # Set up an algorithm dumping the kinematic properties of the electrons:
    if enableKinematicHistograms:
        alg = createAlgorithm( 'CP::KinematicHistAlg', 'ElectronKinematicDumperAlg' + postfix )
        alg.preselection = "&&".join (selectionDecorNames)
        alg.histPattern = 'electron_%VAR%_%SYS%' + postfix
        seq.append( alg, inputPropName = 'input', stageName = 'selection' )

    # Set up the output selection
    if shallowViewOutput or deepCopyOutput:
        selectionDecorNamesOutput = selectionDecorNames[ : ]
        if not ptSelectionOutput:
            selectionDecorNamesOutput.remove(ptSelectionDecoration)

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    if shallowViewOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'ElectronViewFromSelectionAlg' + postfix )
        alg.selection = selectionDecorNamesOutput[ : ]
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Set up the electron efficiency correction algorithm:
    alg = createAlgorithm( 'CP::ElectronEfficiencyCorrectionAlg',
                           'ElectronEfficiencyCorrectionAlg' + postfix )
    alg.preselection = "&&".join (selectionDecorNames)
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgElectronEfficiencyCorrectionTool' )
    alg.scaleFactorDecoration = 'effSF' + postfix + '_%SYS%'
    alg.scaleFactorDecorationRegex = '(^EL_EFF_Reco.*)'
    alg.efficiencyCorrectionTool.RecoKey = "Reconstruction"
    alg.efficiencyCorrectionTool.CorrelationModel = "TOTAL"
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
                    affectingSystematics = '(^EL_EFF_Reco.*)',
                    stageName = 'efficiency' )
        pass

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'ElectronDeepCopyMaker' + postfix )
        alg.selection = selectionDecorNamesOutput[:]
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Return the sequence:
    return seq
