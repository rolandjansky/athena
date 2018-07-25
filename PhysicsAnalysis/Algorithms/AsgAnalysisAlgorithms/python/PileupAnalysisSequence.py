# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makePileupAnalysisSequence( dataType, userPileupConfigs=[], userLumicalcFiles=[] ):
    """Create a PRW analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "PileupAnalysisSequence" )

    muMcFiles = ["dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
                 "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"] + userPileupConfigs
    muDataFiles = ["dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"] + userLumicalcFiles

    # Set up the only algorithm of the sequence:
    alg = createAlgorithm( 'CP::PileupReweightingAlg', 'PileupReweightingAlg' )
    addPrivateTool( alg, 'pileupReweightingTool', 'CP::PileupReweightingTool' )
    alg.pileupReweightingTool.ConfigFiles = muMcFiles
    alg.pileupReweightingTool.LumiCalcFiles = muDataFiles
    alg.pileupReweightingTool.DataScaleFactor = 1./1.09
    alg.pileupReweightingTool.DataScaleFactorUP = 1.
    alg.pileupReweightingTool.DataScaleFactorDOWN = 1./1.18
    seq.append( alg, inputPropName = 'eventInfo',
                outputPropName = 'eventInfoOut',
                affectingSystematics = '(^PRW_.*)' )

    # Return the sequence:
    return seq
