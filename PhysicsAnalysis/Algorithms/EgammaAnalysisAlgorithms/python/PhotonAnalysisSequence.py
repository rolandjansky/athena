# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makePhotonAnalysisSequence (dataType,photonContainer="Photons",quality=0) :
    if not dataType in ["data", "mc", "afii"] :
        raise Exception ("invalid data type: " + dataType)

    sequence = []

    alg = createAlgorithm( 'CP::EgammaCalibrationAndSmearingAlg', 'PhotonCalibrationAndSmearingAlg' )
    addPrivateTool (alg, "calibrationAndSmearingTool", "CP::EgammaCalibrationAndSmearingTool")
    alg.calibrationAndSmearingTool.ESModel = "es2016data_mc15c"
    alg.calibrationAndSmearingTool.decorrelationModel = "1NP_v1"
    if dataType == "afii" :
        alg.calibrationAndSmearingTool.useAFII = 1
        pass
    else :
        alg.calibrationAndSmearingTool.useAFII = 0
        pass
    sequence.append ( {"alg" : alg, "in" : "egammas", "out" : "egammasOut",
                       "sys" : "(^EG_RESOLUTION_.*)|(^EG_SCALE_.*)"} )


    # should this be applied to data?  or to AFII?
    alg = createAlgorithm( 'CP::PhotonShowerShapeFudgeAlg', 'PhotonShowerShapeFudgeAlg' )
    addPrivateTool (alg, "showerShapeFudgeTool", "ElectronPhotonShowerShapeFudgeTool")
    alg.showerShapeFudgeTool.Preselection = 21 # 21 = MC15
    alg.showerShapeFudgeTool.FFCalibFile = "ElectronPhotonShowerShapeFudgeTool/v1/PhotonFudgeFactors.root" #only for rel21 
    sequence.append ( {"alg" : alg, "in" : "photons", "out" : "photonsOut"} )



    alg = createAlgorithm( 'CP::EgammaIsolationCorrectionAlg', 'PhotonIsolationCorrectionAlg' )
    addPrivateTool (alg, "isolationCorrectionTool", "CP::IsolationCorrectionTool")
    if dataType == "data" :
        alg.isolationCorrectionTool.IsMC = 0
        pass
    else :
        alg.isolationCorrectionTool.IsMC = 1
        pass
    sequence.append ( {"alg" : alg, "in" : "egammas", "out" : "egammasOut" } )



    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'AsgPhotonIsEMSelectorAlg' )
    addPrivateTool (alg, "selectionTool", "AsgPhotonIsEMSelector")
    alg.selectionTool.isEMMask = quality
    alg.selectionTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMTightSelectorCutDefs.conf"
    alg.selectionDecoration = "selectEM"
    sequence.append ( {"alg" : alg, "in" : "particles", "out" : "particlesOut"} )



    egMapFile = "PhotonEfficiencyCorrection/2015_2017/rel21.2/Winter2018_Prerec_v1/map0.txt"

    alg = createAlgorithm( 'CP::PhotonEfficiencyCorrectionAlg', 'PhotonEfficiencyCorrectionAlg' )
    addPrivateTool (alg, "efficiencyCorrectionTool", "AsgPhotonEfficiencyCorrectionTool")
    alg.efficiencyCorrectionTool.MapFilePath = egMapFile
    alg.efficiencyCorrectionTool.ForceDataType = 1
    alg.efficiencyDecoration = "effCor"
    if dataType == "afii" :
        alg.efficiencyCorrectionTool.ForceDataType = 3
        pass
    else :
        alg.efficiencyCorrectionTool.ForceDataType = 1
        pass
    alg.outOfValidity = 2 #silent
    alg.outOfValidityDeco = "bad_eff"
    sequence.append ( {"alg" : alg, "in" : "photons", "out" : "photonsOut",
                       "sys" : "(^PH_EFF_.*)"} )



    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'PhotonCutFlowDumperAlg' )
    alg.histPattern = "photon_cflow_%SYS%"
    alg.selection = ["selectEM","bad_eff"]
    alg.selectionNCuts = [1,1]
    sequence.append ( {"alg" : alg, "in" : "input"} )



    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'PhotonViewFromSelectionAlg' )
    alg.selection = ["selectEM","bad_eff"]
    sequence.append ( {"alg" : alg, "in" : "input", "out" : "output", "needOut" : True} )



    alg = createAlgorithm( 'CP::KinematicHistAlg', 'PhotonKinematicDumperAlg' )
    alg.histPattern = "photon_%VAR%_%SYS%"
    sequence.append ( {"alg" : alg, "in" : "input"} )

    return sequence
