# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makePhotonAnalysisSequence( dataType, quality = 0 ):
    """Create a photon analysis algorithm sequence

    Keywrod arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      quality -- The photon quality to require during the selection
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "PhotonAnalysisSequence" )

    # Set up the calibration ans smearing algorithm:
    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg',
                           'PhotonCalibrationAndSmearingAlg' )
    addPrivateTool( alg, 'calibrationAndSmearingTool',
                    'CP::EgammaCalibrationAndSmearingTool' )
    alg.calibrationAndSmearingTool.ESModel = 'es2017_R21_PRE'
    alg.calibrationAndSmearingTool.decorrelationModel = '1NP_v1'
    if dataType == 'afii' :
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

    # Set up the isolation correction algorithm:
    alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg',
                           'PhotonIsolationCorrectionAlg' )
    addPrivateTool( alg, 'isolationCorrectionTool',
                    'CP::IsolationCorrectionTool' )
    if dataType == 'data':
        alg.isolationCorrectionTool.IsMC = 0
    else :
        alg.isolationCorrectionTool.IsMC = 1
        pass
    seq.append( alg, inputPropName = 'egammas', outputPropName = 'egammasOut' )

    # Set up the photon selection algorithm:
    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'PhotonIsEMSelectorAlg' )
    addPrivateTool( alg, 'selectionTool', 'AsgPhotonIsEMSelector' )
    alg.selectionTool.isEMMask = quality
    alg.selectionTool.ConfigFile = \
        'ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMTightSelectorCutDefs.conf'
    alg.selectionDecoration = 'selectEM'
    seq.append( alg, inputPropName = 'particles',
                outputPropName = 'particlesOut' )

    # Set up the photon efficiency correction algorithm:
    alg = createAlgorithm( 'CP::PhotonEfficiencyCorrectionAlg',
                           'PhotonEfficiencyCorrectionAlg' )
    addPrivateTool( alg, 'efficiencyCorrectionTool',
                    'AsgPhotonEfficiencyCorrectionTool' )
    alg.efficiencyCorrectionTool.MapFilePath = \
        'PhotonEfficiencyCorrection/2015_2017/rel21.2/Winter2018_Prerec_v1/map0.txt'
    alg.efficiencyDecoration = 'effCor'
    if dataType == 'afii':
        alg.efficiencyCorrectionTool.ForceDataType = 3
        pass
    else :
        alg.efficiencyCorrectionTool.ForceDataType = 1
        pass
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = 'bad_eff'
    seq.append( alg, inputPropName = 'photons', outputPropName = 'photonsOut',
                affectingSystematics = '(^PH_EFF_.*)' )

    # Set up an algorithm used for debugging the photon selection:
    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg',
                           'PhotonCutFlowDumperAlg' )
    alg.histPattern = 'photon_cflow_%SYS%'
    alg.selection = [ 'selectEM', 'bad_eff' ]
    alg.selectionNCuts = [ 1, 1 ]
    seq.append( alg, inputPropName = 'input' )

    # Set up an algorithm that makes a view container using the selections
    # performed previously:
    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg',
                           'PhotonViewFromSelectionAlg' )
    alg.selection = [ 'selectEM', 'bad_eff' ]
    seq.append( alg, inputPropName = 'input', outputPropName = 'output' )

    # Set up an algorithm dumping the properties of the photons, for debugging:
    alg = createAlgorithm( 'CP::KinematicHistAlg', 'PhotonKinematicDumperAlg' )
    alg.histPattern = 'photon_%VAR%_%SYS%'
    seq.append( alg, inputPropName = 'input' )

    # Return the sequence:
    return seq
