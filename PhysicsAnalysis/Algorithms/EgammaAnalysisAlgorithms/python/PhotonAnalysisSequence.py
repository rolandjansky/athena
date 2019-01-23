# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Framework import(s):
import ROOT

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makePhotonAnalysisSequence( dataType, workingPoint,
                                deepCopyOutput = False,
                                postfix = '',
                                recomputeIsEM = False ):
    """Create a photon analysis algorithm sequence

    Keywrod arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      deepCopyOutput -- If set to 'True', the output containers will be
                        standalone, deep copies (slower, but needed for xAOD
                        output writing)
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      recomputeIsEM -- Whether to rerun the cut-based selection. If not, use derivation flags
    """

    # Make sure we received a valid data type.
    if not dataType in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %' % dataType )

    if postfix != '' :
        postfix = '_' + postfix
        pass

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 2 :
        raise ValueError ('working point should be of format "quality.isolation", not ' + workingPoint)

    qualityWP = splitWP[0]
    isolationWP = splitWP[1]

    if qualityWP == 'Tight' :
        quality = ROOT.egammaPID.PhotonTight
        pass
    elif qualityWP == 'Loose' :
        quality = ROOT.egammaPID.PhotonLoose
        pass
    else :
        raise Exception ('unknown photon quality working point "' + qualityWP + '" should be Tight or Loose');

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "PhotonAnalysisSequence" + postfix )

    # Variables keeping track of the selections being applied.
    selectionDecorNames = []
    selectionDecorCount = []

    # Set up the photon selection algorithm:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonIsEMSelectorAlg' + postfix )
    alg.selectionDecoration = 'selectEM'
    selectionDecorNames.append( alg.selectionDecoration )
    if recomputeIsEM:
        # Rerun the cut-based ID
        addPrivateTool( alg, 'selectionTool', 'AsgPhotonIsEMSelector' )
        alg.selectionTool.isEMMask = quality
        alg.selectionTool.ConfigFile = \
          'ElectronPhotonSelectorTools/offline/20180116/PhotonIsEMTightSelectorCutDefs.conf'
        selectionDecorCount.append( 32 )
    else:
        # Select from Derivation Framework flags
        addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
        dfFlag = 'DFCommonPhotonsIsEM' + qualityWP
        alg.selectionTool.selectionFlags = [ dfFlag ]
        selectionDecorCount.append( 1 )
        pass
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'calibration' )

    # Select electrons only with good object quality.
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonObjectQualityAlg' + postfix )
    alg.selectionDecoration = 'goodOQ'
    addPrivateTool( alg, 'selectionTool', 'CP::EgammaIsGoodOQSelectionTool' )
    alg.selectionTool.Mask = ROOT.xAOD.EgammaParameters.BADCLUSPHOTON
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'calibration' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 1 )

    # Only run subsequent processing on the objects passing all of these cuts.
    # Since these are independent of the photon calibration, and this speeds
    # up the job.
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'PhotonPreSelViewFromSelectionAlg' + postfix )
    alg.selection = selectionDecorNames[ : ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                stageName = 'calibration' )

    # Set up the calibration ans smearing algorithm.
    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg',
                           'PhotonCalibrationAndSmearingAlg' + postfix )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::EgammaCalibrationAndSmearingTool' )
    alg.calibrationAndSmearingTool.ESModel = 'es2017_R21_PRE'
    alg.calibrationAndSmearingTool.decorrelationModel = '1NP_v1'
    if dataType == 'afii':
        alg.calibrationAndSmearingTool.useAFII = 1
    else :
        alg.calibrationAndSmearingTool.useAFII = 0
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                affectingSystematics = '(^EG_RESOLUTION_.*)|(^EG_SCALE_.*)',
                stageName = 'calibration' )

    # should this be applied to data?  or to AFII?
    alg = createAlgorithm( 'CP::PhotonShowerShapeFudgeAlg',
                           'PhotonShowerShapeFudgeAlg' + postfix )
    addPrivateTool( alg, 'showerShapeFudgeTool',
                    'ElectronPhotonShowerShapeFudgeTool' )
    alg.showerShapeFudgeTool.Preselection = 21 # 21 = MC15
    alg.showerShapeFudgeTool.FFCalibFile = \
        'ElectronPhotonShowerShapeFudgeTool/v1/PhotonFudgeFactors.root' #only for rel21
    seq.append( alg, inputPropName = 'photons', outputPropName = 'photonsOut',
                stageName = 'calibration' )

    # Set up the isolation correction algorithm.
    alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg',
                           'PhotonIsolationCorrectionAlg' + postfix )
    addPrivateTool( alg, 'isolationCorrectionTool',
                    'CP::IsolationCorrectionTool' )
    if dataType == 'data':
        alg.isolationCorrectionTool.IsMC = 0
    else:
        alg.isolationCorrectionTool.IsMC = 1
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                stageName = 'selection' )

    # Set up the isolation selection algorithm:
    alg = createAlgorithm( 'CP::EgammaIsolationSelectionAlg',
                           'PhotonIsolationSelectionAlg' + postfix )
    alg.selectionDecoration = 'isolated' + postfix
    addPrivateTool( alg, 'selectionTool', 'CP::IsolationSelectionTool' )
    alg.selectionTool.PhotonWP = isolationWP
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                stageName = 'selection' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 1 )

    # Set up the photon efficiency correction algorithm.
    alg = createAlgorithm( 'CP::PhotonEfficiencyCorrectionAlg',
                           'PhotonEfficiencyCorrectionAlg' + postfix )
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgPhotonEfficiencyCorrectionTool' )
    alg.efficiencyCorrectionTool.MapFilePath = \
        'PhotonEfficiencyCorrection/2015_2017/rel21.2/Winter2018_Prerec_v1/map0.txt'
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
        seq.append( alg, inputPropName = 'photons',
                    outputPropName = 'photonsOut',
                    affectingSystematics = '(^PH_EFF_.*)',
                    stageName = 'efficiency' )
        selectionDecorNames.append( alg.outOfValidityDeco )
        selectionDecorCount.append( 1 )
        pass

    # Set up an algorithm used for debugging the photon selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                           'PhotonCutFlowDumperAlg' + postfix )
    alg.histPattern = 'photon_cflow_%SYS%' + postfix
    alg.selection = selectionDecorNames[ : ]
    alg.selectionNCuts = selectionDecorCount[ : ]
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'PhotonViewFromSelectionAlg' + postfix )
    alg.selection = selectionDecorNames[ : ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                stageName = 'selection' )

    # Set up an algorithm dumping the properties of the photons, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'PhotonKinematicDumperAlg' + postfix )
    alg.histPattern = 'photon_%VAR%_%SYS%' + postfix
    seq.append( alg, inputPropName = 'input',
                stageName = 'selection' )

    # Set up a final deep copy making algorithm if requested:
    if deepCopyOutput:
        alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                               'PhotonDeepCopyMaker' + postfix )
        alg.deepCopy = True
        seq.append( alg, inputPropName = 'input', outputPropName = 'output',
                    stageName = 'selection' )
        pass

    # Return the sequence:
    return seq
