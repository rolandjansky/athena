# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Framework import(s):
import ROOT

# AnaAlgorithm import(s):
from AsgAnalysisAlgorithms.AnalysisObjectSharedSequence import makeSharedObjectSequence
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makePhotonAnalysisSequence( dataType, workingPoint,
                                deepCopyOutput = False,
                                shallowViewOutput = True,
                                postfix = '',
                                recomputeIsEM = False,
                                enableCutflow = False,
                                enableKinematicHistograms = False ):
    """Create a photon analysis algorithm sequence

    Keywrod arguments:
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
      recomputeIsEM -- Whether to rerun the cut-based selection. If not, use derivation flags
      enableCutflow -- Whether or not to dump the cutflow
      enableKinematicHistograms -- Whether or not to dump the kinematic histograms
    """

    # Make sure we received a valid data type.
    if dataType not in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %' % dataType )

    if postfix != '' :
        postfix = '_' + postfix
        pass

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "PhotonAnalysisSequence" + postfix )

    seq.addMetaConfigDefault ("selectionDecorNames", [])
    seq.addMetaConfigDefault ("selectionDecorNamesOutput", [])
    seq.addMetaConfigDefault ("selectionDecorCount", [])

    makePhotonCalibrationSequence (seq, dataType, postfix=postfix)
    makePhotonWorkingPointSequence (seq, dataType, workingPoint, postfix=postfix,
                                    recomputeIsEM = recomputeIsEM)
    makeSharedObjectSequence (seq, deepCopyOutput = deepCopyOutput,
                              shallowViewOutput = shallowViewOutput,
                              postfix = '_Photon' + postfix,
                              enableCutflow = enableCutflow,
                              enableKinematicHistograms = enableKinematicHistograms )

    # Return the sequence:
    return seq





def makePhotonCalibrationSequence( seq, dataType,
                                 postfix = ''):
    """Create photon calibration analysis algorithms

    This makes all the algorithms that need to be run first befor
    all working point specific algorithms and that can be shared
    between the working points.

    Keywrod arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
    """

    # Make sure we received a valid data type.
    if dataType not in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %s' % dataType )

    # Select electrons only with good object quality.
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonObjectQualityAlg' + postfix )
    alg.selectionDecoration = 'goodOQ'
    addPrivateTool( alg, 'selectionTool', 'CP::EgammaIsGoodOQSelectionTool' )
    alg.selectionTool.Mask = ROOT.xAOD.EgammaParameters.BADCLUSPHOTON
    seq.append( alg, inputPropName = 'particles', outputPropName = 'particlesOut',
                stageName = 'calibration',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [1]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    # Set up the calibration ans smearing algorithm.
    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg',
                           'PhotonCalibrationAndSmearingAlg' + postfix )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::EgammaCalibrationAndSmearingTool' )
    alg.calibrationAndSmearingTool.ESModel = 'es2018_R21_v0'
    alg.calibrationAndSmearingTool.decorrelationModel = '1NP_v1'
    if dataType == 'afii':
        alg.calibrationAndSmearingTool.useAFII = 1
    else :
        alg.calibrationAndSmearingTool.useAFII = 0
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut',
                stageName = 'calibration',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    # should this be applied to data?  or to AFII?
    alg = createAlgorithm( 'CP::PhotonShowerShapeFudgeAlg',
                           'PhotonShowerShapeFudgeAlg' + postfix )
    addPrivateTool( alg, 'showerShapeFudgeTool',
                    'ElectronPhotonShowerShapeFudgeTool' )
    alg.showerShapeFudgeTool.Preselection = 22 # Rel 21
    alg.showerShapeFudgeTool.FFCalibFile = \
        'ElectronPhotonShowerShapeFudgeTool/v2/PhotonFudgeFactors.root' #only for rel21
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
                stageName = 'selection',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )
    pass





def makePhotonWorkingPointSequence( seq, dataType, workingPoint, postfix = '',
                                    recomputeIsEM = False):
    """Create photon analysis algorithms for a single working point

    Keywrod arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      recomputeIsEM -- Whether to rerun the cut-based selection. If not, use derivation flags
    """

    # Make sure we received a valid data type.
    if dataType not in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %s' % dataType )

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
        raise Exception ('unknown photon quality working point "' + qualityWP + '" should be Tight or Loose')

    # Set up the photon selection algorithm:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonIsEMSelectorAlg' + postfix )
    alg.selectionDecoration = 'selectEM'
    if recomputeIsEM:
        # Rerun the cut-based ID
        addPrivateTool( alg, 'selectionTool', 'AsgPhotonIsEMSelector' )
        alg.selectionTool.isEMMask = quality
        alg.selectionTool.ConfigFile = \
          'ElectronPhotonSelectorTools/offline/20180116/PhotonIsEMTightSelectorCutDefs.conf'
    else:
        # Select from Derivation Framework flags
        addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
        dfFlag = 'DFCommonPhotonsIsEM' + qualityWP
        alg.selectionTool.selectionFlags = [ dfFlag ]
        pass
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'calibration',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [32 if recomputeIsEM else 1]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    # Set up the isolation selection algorithm:
    alg = createAlgorithm( 'CP::EgammaIsolationSelectionAlg',
                           'PhotonIsolationSelectionAlg' + postfix )
    alg.selectionDecoration = 'isolated' + postfix
    addPrivateTool( alg, 'selectionTool', 'CP::IsolationSelectionTool' )
    alg.selectionTool.PhotonWP = isolationWP
    seq.append( alg, inputPropName = 'egammas',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [1]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )

    # Set up the photon efficiency correction algorithm.
    alg = createAlgorithm( 'CP::PhotonEfficiencyCorrectionAlg',
                           'PhotonEfficiencyCorrectionAlg' + postfix )
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgPhotonEfficiencyCorrectionTool' )
    alg.scaleFactorDecoration = 'effSF' + postfix
    alg.efficiencyCorrectionTool.MapFilePath = \
        'PhotonEfficiencyCorrection/2015_2017/rel21.2/Winter2018_Prerec_v1/map0.txt'
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
                    stageName = 'efficiency',
                    metaConfig = {'selectionDecorNames' : [alg.outOfValidityDeco],
                                  'selectionDecorNamesOutput' : [alg.outOfValidityDeco],
                                  'selectionDecorCount' : [1]},
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])} )
        pass

    pass
