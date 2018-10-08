# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Framework import(s):
import ROOT

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makePhotonAnalysisSequence( dataType, quality = ROOT.egammaPID.PhotonTight,
                                isolationWP = 'FixedCutTight',
                                recomputeIsEM = False ):
    """Create a photon analysis algorithm sequence

    Keywrod arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      quality -- The photon quality to require during the selection
      isolationWP -- The isolation working point to select photons with
      recomputeIsEM -- Whether to rerun the cut-based selection. If not, use derivation flags
    """

    # Make sure we received a valid data type.
    if not dataType in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %' % dataType )

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "PhotonAnalysisSequence" )

    # Variables keeping track of the selections being applied.
    selectionDecorNames = []
    selectionDecorCount = []

    # Set up the photon selection algorithm:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonIsEMSelectorAlg' )
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
        WPnames = { ROOT.egammaPID.PhotonLoose : 'Loose',
                    ROOT.egammaPID.PhotonTight : 'Tight' }
        dfFlag = 'DFCommonPhotonsIsEM' + WPnames[ quality ]
        alg.selectionTool.selectionFlags = [ dfFlag ]
        selectionDecorCount.append( 1 )
        pass
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    # Select electrons only with good object quality.
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonObjectQualityAlg' )
    alg.selectionDecoration = 'goodOQ'
    addPrivateTool( alg, 'selectionTool', 'CP::EgammaIsGoodOQSelectionTool' )
    alg.selectionTool.Mask = ROOT.xAOD.EgammaParameters.BADCLUSPHOTON
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 1 )

    # Only run subsequent processing on the objects passing all of these cuts.
    # Since these are independent of the photon calibration, and this speeds
    # up the job.
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'PhotonPreSelViewFromSelectionAlg' )
    alg.selection = selectionDecorNames[ : ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up the calibration ans smearing algorithm.
    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg',
                           'PhotonCalibrationAndSmearingAlg' )
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
                affectingSystematics = '(^EG_RESOLUTION_.*)|(^EG_SCALE_.*)' )

    # should this be applied to data?  or to AFII?
    alg = createAlgorithm( 'CP::PhotonShowerShapeFudgeAlg',
                           'PhotonShowerShapeFudgeAlg' )
    addPrivateTool( alg, 'showerShapeFudgeTool',
                    'ElectronPhotonShowerShapeFudgeTool' )
    alg.showerShapeFudgeTool.Preselection = 21 # 21 = MC15
    alg.showerShapeFudgeTool.FFCalibFile = \
        'ElectronPhotonShowerShapeFudgeTool/v1/PhotonFudgeFactors.root' #only for rel21
    seq.append( alg, inputPropName = 'photons', outputPropName = 'photonsOut' )

    # Set up the isolation correction algorithm.
    alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg',
                           'PhotonIsolationCorrectionAlg' )
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
                           'PhotonIsolationSelectionAlg' )
    alg.selectionDecoration = 'isolated'
    addPrivateTool( alg, 'selectionTool', 'CP::IsolationSelectionTool' )
    alg.selectionTool.PhotonWP = isolationWP
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut' )
    selectionDecorNames.append( alg.selectionDecoration )
    selectionDecorCount.append( 1 )

    # Set up the photon efficiency correction algorithm.
    alg = createAlgorithm( 'CP::PhotonEfficiencyCorrectionAlg',
                           'PhotonEfficiencyCorrectionAlg' )
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgPhotonEfficiencyCorrectionTool' )
    alg.efficiencyCorrectionTool.MapFilePath = \
        'PhotonEfficiencyCorrection/2015_2017/rel21.2/Winter2018_Prerec_v1/map0.txt'
    alg.efficiencyDecoration = 'effCor'
    if dataType == 'afii':
        alg.efficiencyCorrectionTool.ForceDataType = \
          ROOT.PATCore.ParticleDataType.Fast
    elif dataType == 'mc':
        alg.efficiencyCorrectionTool.ForceDataType = \
          ROOT.PATCore.ParticleDataType.Full
        pass
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff'
    if dataType != 'data':
        seq.append( alg, inputPropName = 'photons',
                    outputPropName = 'photonsOut',
                    affectingSystematics = '(^PH_EFF_.*)' )
        selectionDecorNames.append( alg.outOfValidityDeco )
        selectionDecorCount.append( 1 )
        pass

    # Set up an algorithm used for debugging the photon selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                           'PhotonCutFlowDumperAlg' )
    alg.histPattern = 'photon_cflow_%SYS%'
    alg.selection = selectionDecorNames[ : ]
    alg.selectionNCuts = selectionDecorCount[ : ]
    seq.append( alg, inputPropName = 'input' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'PhotonViewFromSelectionAlg' )
    alg.selection = selectionDecorNames[ : ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up an algorithm dumping the properties of the photons, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'PhotonKinematicDumperAlg' )
    alg.histPattern = 'photon_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input' )

    # Return the sequence:
    return seq
