# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

try:
    from AthenaCommon.Logging import logging
except ImportError:
    import logging
prwlog = logging.getLogger('makePileupAnalysisSequence')

def makePileupAnalysisSequence( dataType, userPileupConfigs=[], userLumicalcFiles=[] , autoConfig=False ):
    """Create a PRW analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
    """

    if dataType not in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( "PileupAnalysisSequence" )

    muMcFiles = userPileupConfigs[:]
    if autoConfig:
        from PileupReweighting.AutoconfigurePRW import getLumiCalcFiles,getMCMuFiles
        userLumicalcFiles = getLumiCalcFiles()
        if len(muMcFiles)==0:
            muMcFiles = getMCMuFiles()
        else:
            prwlog.warning('Sent autoconfig and userPileupConfigs='+str(userPileupConfigs))
            prwlog.warning('Ignoring autoconfig and keeping user-specified files')

    if userLumicalcFiles==[]:
        muDataFiles = ["GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root",
                       "GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root",
                       "GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root",
                       "GoodRunsLists/data18_13TeV/20190708/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root" ]
    else:
        muDataFiles = userLumicalcFiles[:]

    # Set up the only algorithm of the sequence:
    alg = createAlgorithm( 'CP::PileupReweightingAlg', 'PileupReweightingAlg' )
    addPrivateTool( alg, 'pileupReweightingTool', 'CP::PileupReweightingTool' )
    alg.pileupReweightingTool.ConfigFiles = muMcFiles
    if not muMcFiles and dataType != "data":
        prwlog.info("No PRW config files provided. Disabling reweighting")
        # Setting the weight decoration to the empty string disables the reweighting
        alg.pileupWeightDecoration = ""
    alg.pileupReweightingTool.LumiCalcFiles = muDataFiles

    seq.append( alg, inputPropName = {} )

    # Return the sequence:
    return seq
