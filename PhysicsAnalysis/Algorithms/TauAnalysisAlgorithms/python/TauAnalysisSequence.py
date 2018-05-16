# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
import ROOT

def makeTauAnalysisSequence (dataType) :
    if not dataType in ["data", "mc", "afii"] :
        raise Exception ("invalid data type: " + dataType)

    sequence = []

    alg = createAlgorithm( 'CP::TauSmearingAlg', 'TauSmearingAlg' )
    addPrivateTool (alg, "smearingTool", "TauAnalysisTools::TauSmearingTool")
    sequence.append ( {"alg" : alg, "in" : "taus", "out" : "tausOut",
                       "sys" : "(^TAUS_TRUEHADTAU_SME_TES_.*)"} )



    alg = createAlgorithm( 'CP::AsgSelectionAlg', 'TauSelectionAlg' )
    addPrivateTool (alg, "selectionTool", "TauAnalysisTools::TauSelectionTool")
    alg.selectionDecoration = "good_tau"
    sequence.append ( {"alg" : alg, "in" : "particles", "out" : "particlesOut" } )



    alg = createAlgorithm( 'CP::TauEfficiencyCorrectionsAlg', 'TauEfficiencyCorrectionsAlg' )
    addPrivateTool (alg, "efficiencyCorrectionsTool", "TauAnalysisTools::TauEfficiencyCorrectionsTool")
    addPrivateTool (alg, "efficiencyCorrectionsTool.TauSelectionTool", "TauAnalysisTools::TauSelectionTool")
    alg.efficiencyDecoration = "tau_eff"
    # alg.outOfValidity = 2 #silent
    # alg.outOfValidityDeco = "bad_eff"
    sequence.append ( {"alg" : alg, "in" : "taus", "out" : "tausOut",
                       "sys" : "(^TAUS_TRUEELECTRON_EFF_.*)|(^TAUS_TRUEHADTAU_EFF_.*)"} )



    if dataType != "data" :
        alg = createAlgorithm( 'CP::TauTruthMatchingAlg', 'TauTruthMatchingAlg' )
        addPrivateTool (alg, "matchingTool", "TauAnalysisTools::TauTruthMatchingTool")
        alg.matchingTool.WriteTruthTaus = 1
        sequence.append ( {"alg" : alg, "in" : "taus", "out" : "tausOut"} )
        pass



    alg = createAlgorithm( 'CP::ObjectCutFlowHistAlg', 'TauCutFlowDumperAlg' )
    alg.histPattern = "tau_cflow_%SYS%"
    alg.selection = ['good_tau']
    alg.selectionNCuts = [6]
    sequence.append ( {"alg" : alg, "in" : "input"} )



    alg = createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'TauViewFromSelectionAlg' )
    alg.selection = ['good_tau']
    sequence.append ( {"alg" : alg, "in" : "input", "out" : "output", "needOut" : True} )



    alg = createAlgorithm( 'CP::KinematicHistAlg', 'TauKinematicDumperAlg' )
    alg.histPattern = "tau_%VAR%_%SYS%"
    sequence.append ( {"alg" : alg, "in" : "input"} )


    return sequence
