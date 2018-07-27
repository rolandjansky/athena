# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.AnaAlgSequence import AnaAlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool

def makeOverlapAnalysisSequence( dataType,
                                 inputLabel = '', outputLabel = 'overlaps',
                                 linkOverlapObjects = False,
                                 doEleEleOR = False, bJetLabel = '',
                                 boostedLeptons = False ):

    if not dataType in ["data", "mc", "afii"] :
        raise ValueError ("invalid data type: " + dataType)

    # Create the analysis algorithm sequence object:
    seq = AnaAlgSequence( 'OverlapAnalysisSequence' )

    # Create the overlap removal algorithm:
    alg = createAlgorithm( 'CP::OverlapRemovalAlg', 'OverlapRemovalAlg' )

    # Create its main tool, and set its basic properties:
    addPrivateTool( alg, 'overlapTool', 'ORUtils::OverlapRemovalTool' )
    alg.overlapTool.InputLabel = inputLabel
    alg.overlapTool.OutputLabel = outputLabel

    # Since in this setup we pre-configure every overlap removal tool, before
    # knowing exactly which objects we want to remove overlaps between, we need
    # to tell the main tool that we won't be necessarily providing valid
    # pointers for all types that we have set up tools for.
    alg.overlapTool.RequireExpectedPointers = False

    # Set up the electron-electron overlap removal:
    if doEleEleOR:
        addPrivateTool( alg, 'overlapTool.EleEleORT',
                        'ORUtils::EleEleOverlapTool' )
        alg.overlapTool.EleEleORT.InputLabel = inputLabel
        alg.overlapTool.EleEleORT.OutputLabel = outputLabel
        alg.overlapTool.EleEleORT.LinkOverlapObjects = linkOverlapObjects
        pass

    # Set up the electron-muon overlap removal:
    addPrivateTool( alg, 'overlapTool.EleMuORT',
                    'ORUtils::EleMuSharedTrkOverlapTool' )
    alg.overlapTool.EleMuORT.InputLabel = inputLabel
    alg.overlapTool.EleMuORT.OutputLabel = outputLabel
    alg.overlapTool.EleMuORT.LinkOverlapObjects = linkOverlapObjects

    # Set up the electron-(narrow-)jet overlap removal:
    addPrivateTool( alg, 'overlapTool.EleJetORT',
                    'ORUtils::EleJetOverlapTool' )
    alg.overlapTool.EleJetORT.InputLabel = inputLabel
    alg.overlapTool.EleJetORT.OutputLabel = outputLabel
    alg.overlapTool.EleJetORT.LinkOverlapObjects = linkOverlapObjects
    alg.overlapTool.EleJetORT.BJetLabel = bJetLabel
    alg.overlapTool.EleJetORT.UseSlidingDR = boostedLeptons

    # Set up the muon-(narrow-)jet overlap removal:
    addPrivateTool( alg, 'overlapTool.MuJetORT',
                    'ORUtils::MuJetOverlapTool' )
    alg.overlapTool.MuJetORT.InputLabel = inputLabel
    alg.overlapTool.MuJetORT.OutputLabel = outputLabel
    alg.overlapTool.MuJetORT.LinkOverlapObjects = linkOverlapObjects
    alg.overlapTool.MuJetORT.BJetLabel = bJetLabel
    alg.overlapTool.MuJetORT.UseSlidingDR = boostedLeptons

    # Set up the tau-electron overlap removal:
    addPrivateTool( alg, 'overlapTool.TauEleORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.TauEleORT.InputLabel = inputLabel
    alg.overlapTool.TauEleORT.OutputLabel = outputLabel
    alg.overlapTool.TauEleORT.LinkOverlapObjects = linkOverlapObjects
    alg.overlapTool.TauEleORT.DR = 0.2

    # Set up the tau-muon overlap removal:
    addPrivateTool( alg, 'overlapTool.TauMuORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.TauMuORT.InputLabel = inputLabel
    alg.overlapTool.TauMuORT.OutputLabel = outputLabel
    alg.overlapTool.TauMuORT.LinkOverlapObjects = linkOverlapObjects
    alg.overlapTool.TauMuORT.DR = 0.2

    # Set up the tau-(narrow-)jet overlap removal:
    addPrivateTool( alg, 'overlapTool.TauJetORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.TauJetORT.InputLabel = inputLabel
    alg.overlapTool.TauJetORT.OutputLabel = outputLabel
    alg.overlapTool.TauJetORT.LinkOverlapObjects = linkOverlapObjects
    alg.overlapTool.TauJetORT.DR = 0.2

    # Set up the photon-electron overlap removal:
    addPrivateTool( alg, 'overlapTool.PhoEleORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.PhoEleORT.InputLabel = inputLabel
    alg.overlapTool.PhoEleORT.OutputLabel = outputLabel
    alg.overlapTool.PhoEleORT.LinkOverlapObjects = linkOverlapObjects

    # Set up the photon-muon overlap removal:
    addPrivateTool( alg, 'overlapTool.PhoMuORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.PhoMuORT.InputLabel = inputLabel
    alg.overlapTool.PhoMuORT.OutputLabel = outputLabel
    alg.overlapTool.PhoMuORT.LinkOverlapObjects = linkOverlapObjects

    # Set up the photon-(narrow-)jet overlap removal:
    addPrivateTool( alg, 'overlapTool.PhoJetORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.PhoJetORT.InputLabel = inputLabel
    alg.overlapTool.PhoJetORT.OutputLabel = outputLabel
    alg.overlapTool.PhoJetORT.LinkOverlapObjects = linkOverlapObjects

    # Set up the electron-fat-jet overlap removal:
    addPrivateTool( alg, 'overlapTool.EleFatJetORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.EleFatJetORT.InputLabel = inputLabel
    alg.overlapTool.EleFatJetORT.OutputLabel = outputLabel
    alg.overlapTool.EleFatJetORT.LinkOverlapObjects = linkOverlapObjects
    alg.overlapTool.EleFatJetORT.DR = 1.0

    # Set up the (narrow-)jet-fat-jet overlap removal:
    addPrivateTool( alg, 'overlapTool.JetFatJetORT',
                    'ORUtils::DeltaROverlapTool' )
    alg.overlapTool.JetFatJetORT.InputLabel = inputLabel
    alg.overlapTool.JetFatJetORT.OutputLabel = outputLabel
    alg.overlapTool.JetFatJetORT.LinkOverlapObjects = linkOverlapObjects
    alg.overlapTool.JetFatJetORT.DR = 1.0

    # Add the algorithm to the analysis sequence:
    seq.append( alg,
                inputPropName = { 'electrons' : 'electrons',
                                  'muons'     : 'muons',
                                  'jets'      : 'jets',
                                  'taus'      : 'taus',
                                  'photons'   : 'photons',
                                  'fatJets'   : 'fatJets' },
                outputPropName = { 'electrons' : 'electronsOut',
                                   'muons'     : 'muonsOut',
                                   'jets'      : 'jetsOut',
                                   'taus'      : 'tausOut',
                                   'photons'   : 'photonsOut',
                                   'fatJets'   : 'fatJetsOut' } )

    # Return the sequence:
    return seq
