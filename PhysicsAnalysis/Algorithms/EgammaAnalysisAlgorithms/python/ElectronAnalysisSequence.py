# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeElectronAnalysisSequence (dataType,electronContainer="Electrons",isolationWP="GradientLoose",likelihoodWP="LooseLHElectron") :
    if not dataType in ["data", "mc", "afii"] :
        raise Exception ("invalid data type: " + dataType)

    sequence = []

    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg', 'ElectronCalibrationAndSmearingAlg' )
    addPrivateTool (alg, "calibrationAndSmearingTool", "CP::EgammaCalibrationAndSmearingTool")
    alg.calibrationAndSmearingTool.ESModel = "es2017_R21_PRE"
    alg.calibrationAndSmearingTool.decorrelationModel = "1NP_v1"
    if dataType == "afii" :
        alg.calibrationAndSmearingTool.useAFII = 1
        pass
    else :
        alg.calibrationAndSmearingTool.useAFII = 0
        pass
    sequence.append ( {"alg" : alg, "in" : "egammas", "out" : "egammasOut",
                       "sys" : "(^EG_RESOLUTION_.*)|(^EG_SCALE_.*)"} )



    alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg', 'ElectronIsolationCorrectionAlg' )
    addPrivateTool (alg, "isolationCorrectionTool", "CP::IsolationCorrectionTool")
    if dataType == "data" :
        alg.isolationCorrectionTool.IsMC = 0
        pass
    else :
        alg.isolationCorrectionTool.IsMC = 1
        pass
    # if dataType == "afii" :
    #     alg.isolationCorrectionTool.AFII_coor = 1
    #     pass
    # else :
    #     alg.isolationCorrectionTool.AFII_coor = 0
    #     pass
    sequence.append ( {"alg" : alg, "in" : "egammas", "out" : "egammasOut" } )



    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'ElectronLikelihoodAlg' )
    addPrivateTool (alg, "selectionTool", "AsgElectronLikelihoodTool")
    alg.selectionTool.primaryVertexContainer = "PrimaryVertices"
    alg.selectionTool.WorkingPoint = likelihoodWP
    alg.selectionDecoration = "selectLikelihood"
    sequence.append ( {"alg" : alg, "in" : "particles", "out" : "particlesOut"} )
    pass



    alg = createAlgorithm( 'CP::EgammaIsolationSelectionAlg', 'ElectronIsolationSelectionAlg' )
    addPrivateTool (alg, "selectionTool", "CP::IsolationSelectionTool")
    alg.selectionTool.ElectronWP = isolationWP
    sequence.append ( {"alg" : alg, "in" : "egammas", "out" : "egammasOut"} )
    pass



    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'ElectronCutFlowDumperAlg' )
    alg.histPattern = "electron_cflow_%SYS%"
    alg.selection = ["selectLikelihood",'isolated']
    alg.selectionNCuts = [7,1]
    sequence.append ( {"alg" : alg, "in" : "input"} )



    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'ElectronViewFromSelectionAlg' )
    alg.selection = ["selectLikelihood",'isolated']
    sequence.append ( {"alg" : alg, "in" : "input", "out" : "output", "needOut" : True} )



    alg = createAlgorithm( 'CP::KinematicHistAlg', 'ElectronKinematicDumperAlg' )
    alg.histPattern = "electron_%VAR%_%SYS%"
    sequence.append ( {"alg" : alg, "in" : "input"} )

    return sequence
