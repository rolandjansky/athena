# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AsgAnalysisAlgorithms.AnalysisObjectSharedSequence import makeSharedObjectSequence
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
import ROOT

def makeMuonAnalysisSequence( dataType, workingPoint,
                              deepCopyOutput = False,
                              shallowViewOutput = True,
                              postfix = '',
                              ptSelectionOutput = False,
                              qualitySelectionOutput = True,
                              enableCutflow = False,
                              enableKinematicHistograms = False ):
    """Create a muon analysis algorithm sequence

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
      ptSelectionOutput -- Whether or not to apply pt selection when creating
                           output containers.
      qualitySelectionOutput -- Whether or not to apply muon quality selection
                                when creating output containers.
      enableCutflow -- Whether or not to dump the cutflow
      enableKinematicHistograms -- Whether or not to dump the kinematic histograms
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    if postfix != '' :
        postfix = '_' + postfix
        pass

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "MuonAnalysisSequence" + postfix )

    seq.addMetaConfigDefault ("selectionDecorNames", [])
    seq.addMetaConfigDefault ("selectionDecorNamesOutput", [])
    seq.addMetaConfigDefault ("selectionDecorCount", [])

    makeMuonCalibrationSequence (seq, dataType, postfix=postfix,
                                 ptSelectionOutput = ptSelectionOutput)
    makeMuonWorkingPointSequence (seq, dataType, workingPoint, postfix=postfix,
                                  qualitySelectionOutput = qualitySelectionOutput)
    makeSharedObjectSequence (seq, deepCopyOutput = deepCopyOutput,
                              shallowViewOutput = shallowViewOutput,
                              postfix = '_Muon' + postfix,
                              enableCutflow = enableCutflow,
                              enableKinematicHistograms = enableKinematicHistograms )

    # Return the sequence:
    return seq





def makeMuonCalibrationSequence( seq, dataType,
                                 postfix = '', ptSelectionOutput = False):
    """Create muon calibration analysis algorithms

    This makes all the algorithms that need to be run first befor
    all working point specific algorithms and that can be shared
    between the working points.

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      ptSelectionOutput -- Whether or not to apply pt selection when creating
                           output containers.
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Set up the eta-cut on all muons prior to everything else
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'MuonEtaCutAlg' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.maxEta = 2.5
    alg.selectionDecoration = 'selectEta' + postfix + ',as_bits'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [2]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})

    # Set up the track selection algorithm:
    alg = createAlgorithm( 'CP::AsgLeptonTrackSelectionAlg',
                           'MuonTrackSelectionAlg' + postfix )
    alg.selectionDecoration = 'trackSelection' + postfix + ',as_bits'
    alg.maxD0Significance = 3
    alg.maxDeltaZ0SinTheta = 0.5
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration],
                              'selectionDecorCount' : [3]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})

    # Set up the muon calibration and smearing algorithm:
    alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg',
                           'MuonCalibrationAndSmearingAlg' + postfix )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::MuonCalibrationPeriodTool' )
    alg.calibrationAndSmearingTool.calibrationMode = 2 # choose ID+MS with no sagitta bias
    seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                stageName = 'calibration',
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})

    # Set up the the pt selection
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'MuonPtCutAlg' + postfix )
    alg.selectionDecoration = 'selectPt' + postfix + ',as_bits'
    addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
    alg.selectionTool.minPt = 3e3
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration] if ptSelectionOutput else [],
                              'selectionDecorCount' : [2]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})
    pass





def makeMuonWorkingPointSequence( seq, dataType, workingPoint, postfix = '',
                                  qualitySelectionOutput = True):
    """Create muon analysis algorithms for a single working point

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      workingPoint -- The working point to use
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      qualitySelectionOutput -- Whether or not to apply muon quality selection
                                when creating output containers.
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 2 :
        raise ValueError ('working point should be of format "quality.isolation", not ' + workingPoint)

    sfWorkingPoint = splitWP[0]
    if splitWP[0] == 'Tight' :
        quality = ROOT.xAOD.Muon.Tight
        pass
    elif splitWP[0] == 'Medium' :
        quality = ROOT.xAOD.Muon.Medium        
        pass
    elif splitWP[0] == 'Loose' :
        quality = ROOT.xAOD.Muon.Loose        
        pass
    elif splitWP[0] == 'VeryLoose' :
        quality = ROOT.xAOD.Muon.VeryLoose        
        pass
    elif splitWP[0] == 'HighPt' :
        quality = 4
        pass
    elif splitWP[0] == 'LowPtEfficiency' :
        quality = 5
        pass
    else :
        raise ValueError ("invalid muon quality: \"" + splitWP[0] +
                          "\", allowed values are Tight, Medium, Loose, " +
                          "VeryLoose, HighPt, LowPtEfficiency")

    isoWP = splitWP[1]

    if not splitWP[1] in ["Iso", "NonIso"] :
        raise ValueError ('invalid muon isolation \"' + splitWP[1] +
                          '\", allowed values are Iso, NonIso')

    # Setup the muon quality selection
    alg = createAlgorithm( 'CP::MuonSelectionAlgV2',
                           'MuonSelectionAlg' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::MuonSelectionTool' )
    alg.selectionTool.MuQuality = quality
    alg.selectionDecoration = 'good_muon' + postfix + ',as_bits'
    alg.badMuonVetoDecoration = 'is_bad' + postfix + ',as_char'
    seq.append( alg, inputPropName = 'muons',
                stageName = 'selection',
                metaConfig = {'selectionDecorNames' : [alg.selectionDecoration],
                              'selectionDecorNamesOutput' : [alg.selectionDecoration] if qualitySelectionOutput else [],
                              'selectionDecorCount' : [4]},
                dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})

    # Set up the isolation calculation algorithm:
    if isoWP != 'NonIso' :
        alg = createAlgorithm( 'CP::MuonIsolationAlg',
                               'MuonIsolationAlg' + postfix )
        addPrivateTool( alg, 'isolationTool', 'CP::IsolationSelectionTool' )
        alg.isolationDecoration = 'isolated_muon' + postfix + ',as_bits'
        alg.isolationTool.MuonWP = isoWP
        seq.append( alg, inputPropName = 'muons', outputPropName = 'muonsOut',
                    stageName = 'selection',
                    metaConfig = {'selectionDecorNames' : [alg.isolationDecoration],
                                  'selectionDecorNamesOutput' : [alg.isolationDecoration],
                                  'selectionDecorCount' : [1]},
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})
        pass

    # Set up the efficiency scale factor calculation algorithm:
    alg = createAlgorithm( 'CP::MuonEfficiencyScaleFactorAlg',
                           'MuonEfficiencyScaleFactorAlg' + postfix )
    addPrivateTool( alg, 'efficiencyScaleFactorTool',
                    'CP::MuonEfficiencyScaleFactors' )
    alg.scaleFactorDecoration = 'muon_effSF' + postfix + "_%SYS%"
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff' + postfix
    alg.efficiencyScaleFactorTool.WorkingPoint = sfWorkingPoint
    if dataType != 'data':
        seq.append( alg, inputPropName = 'muons',
                    stageName = 'efficiency',
                    dynConfig = {'preselection' : lambda meta : "&&".join (meta["selectionDecorNames"])})

    # Set up an algorithm used for decorating baseline muon selection:
    alg = createAlgorithm( 'CP::AsgSelectionAlg',
                           'MuonSelectionSummary' + postfix )
    addPrivateTool( alg, 'selectionTool', 'CP::AsgFlagSelectionTool' )
    alg.selectionDecoration = 'baselineSelection' + postfix + ',as_char'
    seq.append( alg, inputPropName = 'particles',
                stageName = 'selection',
                dynConfig = {'selectionTool.selectionFlags' : lambda meta : meta["selectionDecorNames"][:]})
    pass
