# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makePileupAnalysisSequence (dataType) :
    if not dataType in ["data", "mc", "afii"] :
        raise Exception ("invalid data type: " + dataType)

    sequence = []

    muMcFiles = ["dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
                 "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"]
    muDataFiles = ["dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"]

    alg = createAlgorithm( 'CP::PileupReweightingAlg', 'PileupReweightingAlg' )
    addPrivateTool (alg, "pileupReweightingTool", "CP::PileupReweightingTool")
    alg.pileupReweightingTool.ConfigFiles = muMcFiles
    alg.pileupReweightingTool.LumiCalcFiles = muDataFiles
    alg.pileupReweightingTool.DataScaleFactor = 1./1.09
    alg.pileupReweightingTool.DataScaleFactorUP = 1.
    alg.pileupReweightingTool.DataScaleFactorDOWN = 1./1.18
    sequence.append ( {"alg" : alg, "in" : "eventInfo", "out" : "eventInfoOut",
                       "sys" : "(^PRW_.*)"} )

    return sequence
