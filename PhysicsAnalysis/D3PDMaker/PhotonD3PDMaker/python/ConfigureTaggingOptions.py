# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import CaloD3PDMaker 
import JetD3PDMaker
import JetTagD3PDMaker
from JetD3PDMaker.JetD3PDObject                      import JetD3PDObject
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags
from RecExConfig.RecFlags                            import rec
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from AthenaCommon.Constants                          import WARNING, INFO, ERROR, VERBOSE
from AthenaCommon.Include                            import include

from JetTagD3PDMaker.AddBTagD3PDInfo                 import addBTagInfoToJetObject
from JetTagD3PDMaker.JetTagD3PDMakerKeys             import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags            import JetTagD3PDFlags

def ConfigureTaggingOptions(alltruthjets,
                            JetTagD3PD_JetFitterCharm = True,
                            JetTagD3PD_MV3            = True):
    
    if JetTagD3PD_MV3:
        JetTagD3PDFlags.Taggers+=["MV3_bVSu","MV3_bVSc","MV3_cVSu","MV3_bVSuFlip","MV3_bVScFlip","MV3_cVSuFlip"]
        
    if not JetTagD3PD_JetFitterCharm:
        JetTagD3PDFlags.JetFitterCharmTagInfo=False
    else:
        JetTagD3PDFlags.Taggers+=["JetFitterCharm","JetFitterCharmNeg"]

    addBTagInfoToJetObject(JetD3PDObject, 200)

    from BTagging.BTaggingFlags import BTaggingFlags
    
    BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt4Truth->AntiKt4TopoEM" ]
    BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt4TruthWithMuNoInt->AntiKt4TopoEM" ]
    BTaggingFlags.Runmodus  = "analysis"
    BTaggingFlags.Active=True

    from JetRec.JetRecFlags import jetFlags
    jetFlags.noStandardConfig = True
    jetFlags.doBTagging = False

    BTaggingFlags.OutputLevel              = WARNING
    BTaggingFlags.ConvertParticleJets      = False
    BTaggingFlags.JetFitterNN              = True

    BTaggingFlags.doJetTagNtuple           = False
    BTaggingFlags.doJetTagSlimNtuple       = False
    BTaggingFlags.doJetTagFakeRateNtuple   = False
    BTaggingFlags.doJetTagEfficiencyNtuple = False

    BTaggingFlags.NewGbbNNTag              = True
    BTaggingFlags.JetFitterCharm           = JetTagD3PD_JetFitterCharm
    BTaggingFlags.JetFitterCharmNeg        = JetTagD3PD_JetFitterCharm
    BTaggingFlags.MV3_bVSu                 = JetTagD3PD_MV3
    BTaggingFlags.MV3_bVSc                 = JetTagD3PD_MV3
    BTaggingFlags.MV3_cVSu                 = JetTagD3PD_MV3
    BTaggingFlags.MV3_bVSuFlip             = JetTagD3PD_MV3
    BTaggingFlags.MV3_bVScFlip             = JetTagD3PD_MV3
    BTaggingFlags.MV3_cVSuFlip             = JetTagD3PD_MV3
    BTaggingFlags.GbbNNTag                 = False
    BTaggingFlags.QGTag                    = False # causes FPE's
    BTaggingFlags.writeSecondaryVertices   = True

    BTaggingFlags.SoftMuChi2               = True
    BTaggingFlags.SecondSoftMu             = False
    BTaggingFlags.SecondSoftMuChi2         = False

    BTaggingFlags.IP1D                     = False

    JetTagD3PD_calibTaggers = True
    BTaggingFlags.IP2DFlip             = JetTagD3PD_calibTaggers
    BTaggingFlags.IP2DPos              = JetTagD3PD_calibTaggers
    BTaggingFlags.IP2DNeg              = JetTagD3PD_calibTaggers
    BTaggingFlags.IP2DSpc              = JetTagD3PD_calibTaggers
    BTaggingFlags.IP2DSpcFlip          = JetTagD3PD_calibTaggers
    BTaggingFlags.IP2DSpcPos           = JetTagD3PD_calibTaggers
    BTaggingFlags.IP2DSpcNeg           = JetTagD3PD_calibTaggers
    BTaggingFlags.IP3DFlip             = JetTagD3PD_calibTaggers
    BTaggingFlags.IP3DPos              = JetTagD3PD_calibTaggers
    BTaggingFlags.IP3DNeg              = JetTagD3PD_calibTaggers
    BTaggingFlags.IP3DSpc              = JetTagD3PD_calibTaggers
    BTaggingFlags.IP3DSpcFlip          = JetTagD3PD_calibTaggers
    BTaggingFlags.IP3DSpcPos           = JetTagD3PD_calibTaggers
    BTaggingFlags.IP3DSpcNeg           = JetTagD3PD_calibTaggers
    BTaggingFlags.SV1Flip              = JetTagD3PD_calibTaggers
    BTaggingFlags.SV2Flip              = JetTagD3PD_calibTaggers
    BTaggingFlags.JetProbFlip          = False
    BTaggingFlags.SoftEl               = False ## no calib
    BTaggingFlags.JetFitterNNFlip      = JetTagD3PD_calibTaggers
    BTaggingFlags.TrackCounting        = JetTagD3PD_calibTaggers
    BTaggingFlags.TrackCountingFlip    = JetTagD3PD_calibTaggers
    BTaggingFlags.MV1Flip              = JetTagD3PD_calibTaggers
    BTaggingFlags.MV2Flip              = JetTagD3PD_calibTaggers
    BTaggingFlags.JetProb              = False
    
    # Truth Jet information
    if rec.doTruth():
            
        if PhotonD3PDMakerFlags.RetagTruthBJets():
            if PhotonD3PDMakerFlags.FillR0p6Jets():
                BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt6Truth->AntiKt6TopoEM" ]
                BTaggingFlags.CalibrationChannelAliases+=[ "AntiKt6TruthWithMuNoInt->AntiKt6TopoEM" ]

            for truthjets in alltruthjets:
                modtruthjets = truthjets[:-4]
                BTaggingFlags.Jets             += [modtruthjets]
                BTaggingFlags.JetsWithInfoPlus += [modtruthjets]
                
            
    include( "BTagging/BTagging_LoadTools.py" )

    if PhotonD3PDMakerFlags.RetagTruthBJets():
        # Now, when we loaded the tools, the new jet collections didn't exist in the input file.
        # The tools checked the input file to see what collections it would retag.  So we need
        # to explicitly add the new collections back into the b-tagging lists.
        for truthjets in alltruthjets:
            modtruthjets = truthjets[:-4]
            BTaggingFlags.RetagJets        += [modtruthjets]
