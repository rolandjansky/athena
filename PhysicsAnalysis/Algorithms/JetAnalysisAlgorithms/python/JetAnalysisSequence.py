# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeJetAnalysisSequence (jetContainer,dataType,runJvtUpdate=True,runJvtEfficiency=True,runJetSmearing=True) :
    if not dataType in ["data", "mc", "afii"] :
        raise Exception ("invalid data type: " + dataType)

    sequence = []

    jetCollection = jetContainer[0:-4]
    if dataType == "afii" :
        configFile = "JES_MC16Recommendation_AFII_EMTopo_April2018_rel21.config"
        pass
    else :
        configFile = "JES_data2017_2016_2015_Recommendation_Feb2018_rel21.config"
        pass
    if dataType == "data" :
        calibSeq = "JetArea_Residual_EtaJES_GSC_Insitu"
        pass
    else :
        calibSeq = "JetArea_Residual_EtaJES_GSC"
        pass

    alg = createAlgorithm( 'CP::JetCalibrationAlg', 'JetCalibrationAlg' )
    addPrivateTool (alg, "calibrationTool", "JetCalibrationTool")
    alg.calibrationTool.CalibArea = "00-04-81"
    alg.calibrationTool.JetCollection = jetCollection
    alg.calibrationTool.ConfigFile = configFile
    alg.calibrationTool.CalibSequence = calibSeq
    if dataType == "data" :
        alg.calibrationTool.IsData = 1
        pass
    else :
        alg.calibrationTool.IsData = 0
        pass
    sequence.append ( {"alg" : alg, "in" : "jets", "out" : "jetsOut"} )


    uncertConfigFile = "rel21/Moriond2018/R4_StrongReduction_Scenario1.config"

    alg = createAlgorithm( 'CP::JetUncertaintiesAlg', 'JetUncertaintiesAlg' )
    addPrivateTool (alg, "uncertaintiesTool", "JetUncertaintiesTool")
    alg.uncertaintiesTool.JetDefinition = jetCollection
    alg.uncertaintiesTool.ConfigFile = uncertConfigFile
    alg.uncertaintiesTool.CalibArea = "CalibArea-03"
    if dataType == "afii" :
        alg.uncertaintiesTool.MCType = "AFII"
        pass
    else :
        alg.uncertaintiesTool.MCType = "MC16"
        pass
    sequence.append ( {"alg" : alg, "in" : "jets", "out" : "jetsOut",
                       "sys" : "(^JET_RelativeNonClosure.*)|(^JET_GroupedNP.*)|(^JET_EtaIntercalibration.*)"} )



    if runJetSmearing :
        jerFile = "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root"

        alg = createAlgorithm( 'CP::JetSmearingAlg', 'JetSmearingAlg' )
        addPrivateTool (alg, "smearingTool", "JERSmearingTool")
        addPrivateTool (alg, "smearingTool.JERTool", "JERTool")
        if dataType == "data" :
            alg.smearingTool.isMC = 0
            pass
        else :
            alg.smearingTool.isMC = 1
            pass
        alg.smearingTool.ApplyNominalSmearing = 0
        alg.smearingTool.SystematicMode = "Simple"
        alg.smearingTool.JERTool.PlotFileName = jerFile
        alg.smearingTool.JERTool.CollectionName = jetContainer
        sequence.append ( {"alg" : alg, "in" : "jets", "out" : "jetsOut",
                           "sys" : "(^JET_JER_.*)"} )
        pass



    alg = createAlgorithm( 'CP::JetSelectionAlg', 'JetCleaningAlg' )
    addPrivateTool (alg, "selectionTool", "JetCleaningTool")
    alg.selectionTool.CutLevel = "LooseBad"
    alg.selectionTool.DoUgly = 0
    sequence.append ( {"alg" : alg, "in" : "jets", "out" : "jetsOut"} )



    if runJvtUpdate :
        jvtFile = "JetMomentTools/JVTlikelihood_20140805.root"

        alg = createAlgorithm( 'CP::JvtUpdateAlg', 'JvtUpdateAlg' )
        addPrivateTool (alg, "jvtTool", "JetVertexTaggerTool")
        alg.jvtTool.JVTFileName = jvtFile
        sequence.append ( {"alg" : alg, "in" : "jets", "out" : "jetsOut"} )
        pass



    alg = createAlgorithm( 'CP::JvtEfficiencyAlg', 'JvtEfficiencyAlg' )
    addPrivateTool (alg, "efficiencyTool", "CP::JetJvtEfficiency")
    alg.selection = "jvt_selection"
    if runJvtEfficiency :
        alg.efficiency = "jvt_efficiency"
        alg.outOfValidity = "no_jvt"
        alg.skipBadEfficiency = "0"
        # disabled, doesn't seem to work
        #alg.efficiencyTool.WorkingPoint = "Default"
        pass
    sequence.append ( {"alg" : alg, "in" : "jets", "out" : "jetsOut",
                       "sys" : "(^JET_JvtEfficiency$)|(^JET_fJvtEfficiency$)"} )



    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'JetCutFlowDumperAlg' )
    alg.histPattern = "jet_cflow_%SYS%"
    alg.selection = ['clean_jet','jvt_selection']
    alg.selectionNCuts = [1,1]
    sequence.append ( {"alg" : alg, "in" : "input"} )



    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'JetViewFromSelectionAlg' )
    alg.selection = ['clean_jet','jvt_selection']
    sequence.append ( {"alg" : alg, "in" : "input", "out" : "output", "needOut" : True} )



    alg = createAlgorithm( 'CP::KinematicHistAlg', 'JetKinematicDumperAlg' )
    alg.histPattern = "jet_%VAR%_%SYS%"
    sequence.append ( {"alg" : alg, "in" : "input"} )

    return sequence
