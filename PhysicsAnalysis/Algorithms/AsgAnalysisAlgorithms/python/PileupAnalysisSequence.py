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

    muMcFiles = userPileupConfigs[:]

    if userLumicalcFiles==[]:
        muDataFiles = ["GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root",
                   "GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root",
                   "GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root" ]
    else:
        muDataFiles = userLumicalcFiles[:]

    # Set up the only algorithm of the sequence:
    alg = createAlgorithm( 'CP::PileupReweightingAlg', 'PileupReweightingAlg' )
    addPrivateTool( alg, 'pileupReweightingTool', 'CP::PileupReweightingTool' )
    alg.pileupReweightingTool.ConfigFiles = muMcFiles
    alg.pileupReweightingTool.LumiCalcFiles = muDataFiles

    seq.append( alg, inputPropName = 'eventInfo',
                outputPropName = 'eventInfoOut',
                affectingSystematics = '(^PRW_.*)' )

    # Return the sequence:
    return seq
