# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeOverlapAnalysisSequence( dataType ):
    """Create an overlap removal analysis algorithm sequence

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
    """

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AlgSequence( "OverlapAnalysisSequence" )

    # Set up the only algorithm of the sequence:
    alg = createAlgorithm ('CP::OverlapRemovalAlg', 'OverlapRemovalAlg')
    addPrivateTool( alg, 'overlapTool', 'ORUtils::OverlapRemovalTool' )
    addPrivateTool( alg, 'overlapTool.EleJetORT', 'ORUtils::EleJetOverlapTool')
    alg.overlapTool.InputLabel = ""
    alg.overlapTool.EleJetORT.InputLabel = ""
    alg.electrons = "Electrons"
    alg.electronsOut = "MyElectrons"
    alg.jets = "AntiKt4EMTopoJets"
    alg.jetsOut = "MyAntiKt4EMTopoJets"
    seq += alg

    # Return the sequence:
    return seq
