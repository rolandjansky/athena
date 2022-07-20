# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Framework import(s):
import ROOT

# AnaAlgorithm import(s):
from AsgAnalysisAlgorithms.AnalysisObjectSharedSequence import makeSharedObjectSequence
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

# E/gamma import(s).
from xAODEgamma.xAODEgammaParameters import xAOD

import PATCore.ParticleDataType

def makePhotonAnalysisSequence( dataType, workingPoint,
                                deepCopyOutput = False,
                                shallowViewOutput = True,
                                postfix = '',
                                crackVeto = False,
                                enableCleaning = True,
                                cleaningAllowLate = False,
                                recomputeIsEM = False,
                                ptSelectionOutput = False,
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
      crackVeto -- Whether or not to perform eta crack veto
      enableCleaning -- Enable photon cleaning
      cleaningAllowLate -- Whether to ignore timing information in cleaning.
      recomputeIsEM -- Whether to rerun the cut-based selection. If not, use derivation flags
      ptSelectionOutput -- Whether or not to apply pt selection when creating
                           output containers.
      enableCutflow -- Whether or not to dump the cutflow
      enableKinematicHistograms -- Whether or not to dump the kinematic histograms
    """

    # Make sure we received a valid data type.
    if dataType not in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %s' % dataType )

    if postfix != '' :
        postfix = '_' + postfix
        pass

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "PhotonAnalysisSequence" + postfix )

    seq.addMetaConfigDefault ("selectionDecorNames", [])
    seq.addMetaConfigDefault ("selectionDecorNamesOutput", [])
    seq.addMetaConfigDefault ("selectionDecorCount", [])

    makePhotonCalibrationSequence (seq, dataType, postfix = postfix,
                                   crackVeto = crackVeto,
                                   enableCleaning = enableCleaning, cleaningAllowLate = cleaningAllowLate,
                                   recomputeIsEM = recomputeIsEM,
                                   ptSelectionOutput = ptSelectionOutput)
    makePhotonWorkingPointSequence (seq, dataType, workingPoint, postfix = postfix,
                                    recomputeIsEM = recomputeIsEM )
    makeSharedObjectSequence (seq, deepCopyOutput = deepCopyOutput,
                              shallowViewOutput = shallowViewOutput,
                              postfix = '_Photon' + postfix,
                              enableCutflow = enableCutflow,
                              enableKinematicHistograms = enableKinematicHistograms )

    # Return the sequence:
    return seq





def makePhotonCalibrationSequence( seq, dataType,
                                   postfix = '',
                                   crackVeto = False,
                                   enableCleaning = True,
                                   cleaningAllowLate = False,
                                   recomputeIsEM = False,
                                   ptSelectionOutput = False ):
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
      crackVeto -- Whether or not to perform eta crack veto
      enableCleaning -- Enable photon cleaning
      cleaningAllowLate -- Whether to ignore timing information in cleaning.
      recomputeIsEM -- Whether to rerun the cut-based selection. If not, use derivation flags
      ptSelectionOutput -- Whether or not to apply pt selection when creating
                           output containers.
    """

    # Make sure we received a valid data type.
    if dataType not in [ 'data', 'mc', 'afii' ]:
        raise ValueError( 'Invalid data type: %s' % dataType )

    cleaningWP = 'NoTime' if cleaningAllowLate else ''

    # Set up the eta-cut on all photons prior to everything else
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonEtaCutAlg' + postfix )
    alg.selectionDecoration = 'selectEta' + postfix + ',as_bits'
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.maxEta = 2.37
    if crackVeto:
        alg.selectionTool.etaGapLow = 1.37
        alg.selectionTool.etaGapHigh = 1.52
    alg.selectionTool.useClusterEta = True
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'calibration',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [5 if crackVeto else 4]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )

    # Setup shower shape fudge
    if recomputeIsEM and dataType == 'mc':
        alg = createAlgorithm( 'CP::PhotonShowerShapeFudgeAlg',
                               'PhotonShowerShapeFudgeAlg' + postfix )
        addPrivateTool( alg, 'showerShapeFudgeTool',
                        'ElectronPhotonShowerShapeFudgeTool' )
        alg.showerShapeFudgeTool.Preselection = 22 # Rel 21
        alg.showerShapeFudgeTool.FFCalibFile = \
            'ElectronPhotonShowerShapeFudgeTool/v2/PhotonFudgeFactors.root' # only for rel21
        seq.append( alg, inputPropName = 'photons', outputPropName = 'photonsOut',
                    stageName = 'calibration',
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )
        pass

    # Select photons only with good object quality.
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonObjectQualityAlg' + postfix )
    alg.selectionDecoration = 'goodOQ,as_bits'
    addPrivateTool( alg, 'selectionTool', 'CP::EgammaIsGoodOQSelectionTool' )
    alg.selectionTool.Mask = xAOD.EgammaParameters.BADCLUSPHOTON
    seq.append( alg, inputPropName = 'particles',
                stageName = 'calibration',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [1]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )

    # Select clean photons
    if enableCleaning:
        alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonCleaningAlg' + postfix)
        addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
        alg.selectionDecoration = 'isClean,as_bits'
        alg.selectionTool.selectionFlags = ['DFCommonPhotonsCleaning' + cleaningWP]
        seq.append( alg, inputPropName = 'particles',
                    stageName = 'calibration',
                    metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                                'selectionDecorNamesOutput' : [alg.selectionDecoration],
                                'selectionDecorCount' : [1]},
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )

    # Do calibration
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
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )

    # Set up the the pt selection
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonPtCutAlg' + postfix )
    alg.selectionDecoration = 'selectPt' + postfix + ',as_bits'
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.minPt = 10e3
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration] if ptSelectionOutput else [],
                              'selectionDecorCount' : [2]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )

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
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )
    pass





def makePhotonWorkingPointSequence( seq, dataType, workingPoint, postfix = '',
                                    recomputeIsEM = False ):
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
    alg.selectionDecoration = 'selectEM,as_bits'
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
                stageName = 'calibration',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [32 if recomputeIsEM else 1]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )

    # Set up the isolation selection algorithm:
    if isolationWP != 'NonIso' :
        alg = createAlgorithm( 'CP::EgammaIsolationSelectionAlg',
                               'PhotonIsolationSelectionAlg' + postfix )
        alg.selectionDecoration = 'isolated' + postfix + ',as_bits'
        addPrivateTool( alg, 'selectionTool', 'CP::IsolationSelectionTool' )
        alg.selectionTool.PhotonWP = isolationWP
        seq.append( alg, inputPropName = 'egammas',
                    stageName = 'selection',
                    metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                                  'selectionDecorNamesOutput' : [alg.selectionDecoration],
                                  'selectionDecorCount' : [1]},
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )
        pass

    # Set up an algorithm used for decorating baseline photon selection:
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'PhotonSelectionSummary' + postfix )
    alg.selectionDecoration = 'baselineSelection' + postfix + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [1]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )

    # Set up the photon efficiency correction algorithm.
    alg = createAlgorithm( 'CP::PhotonEfficiencyCorrectionAlg',
                           'PhotonEfficiencyCorrectionAlg' + postfix )
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgPhotonEfficiencyCorrectionTool' )
    alg.scaleFactorDecoration = 'ph_effSF' + postfix + '_%SYS%'
    if dataType == 'afii':
        alg.efficiencyCorrectionTool.ForceDataType = \
          PATCore.ParticleDataType.Full  # no AFII ID SFs for now
    elif dataType == 'mc':
        alg.efficiencyCorrectionTool.ForceDataType = \
          PATCore.ParticleDataType.Full
        pass
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff' + postfix
    if dataType != 'data':
        seq.append( alg, inputPropName = 'photons',
                    stageName = 'efficiency',
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNamesOutput"])} )
        pass

    pass
