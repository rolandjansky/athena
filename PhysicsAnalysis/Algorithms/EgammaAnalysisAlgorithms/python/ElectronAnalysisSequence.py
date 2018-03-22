# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeElectronAnalysisSequence (dataType,electronContainer="Electrons") :
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



    alg = createAlgorithm( 'CP::KinematicHistAlg', 'ElectronKinematicDumperAlg' )
    alg.histPattern = "electron_%VAR%_%SYS%"
    sequence.append ( {"alg" : alg, "in" : "input"} )

    return sequence
