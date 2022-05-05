# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.Enums import LHCPeriod

Run1Grades = [ "Good", "BlaShared", "PixShared", "SctShared", "0HitBLayer" ]
Run2Grades = [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
               "0HitInExp", "0HitIn",
               "0HitNInExp", "0HitNIn",
               "InANDNInShared", "PixShared", "SctShared",
               "InANDNInSplit", "PixSplit",
               "Good" ]
Run4Grades = [ "A01","A02","A03","A04","A05","A06","A07","A08","A14_1","A14_2","A14_3","A14_4",
               "B01","B02","B03","B04","B05","B06","B07","B08","B14_1","B14_2","B14_3","B14_4",
               "C01","C02030405","C06","C07","C08","C14_1","C14_2","C14_3","C14_4" ]
calibrationChannelAliases = [
    "AntiKt4EMTopo->AntiKt4EMTopo,AntiKt4EMPFlow",
    "AntiKt4EMPFlow->AntiKt4EMPFlow,AntiKt4EMTopo",
    "AntiKt4HI->AntiKt4HI,AntiKt4EMPFlow,AntiKt4EMTopo,AntiKt4LCTopo",
    "AntiKtVR30Rmax4Rmin02PV0Track->AntiKtVR30Rmax4Rmin02PV0Track,AntiKt4EMPFlow,AntiKt4EMTopo",
    "AntiKt4PFlowCustomVtx->AntiKt4EMTopo",
]

def getGrades(flags):
    if flags.GeoModel.Run is LHCPeriod.Run1:
        return Run1Grades
    elif flags.GeoModel.Run in [LHCPeriod.Run2, LHCPeriod.Run3]:
        return Run2Grades
    else:
        return Run4Grades


def getTaggerList(flags):
    # NOTE: MV2c10 is deprecated but something in trigger is asking
    # for it... maybe online monitoring?
    base = ['IP2D','IP3D','SV1','JetFitterNN']
    flip = ['IP2DNeg', 'IP3DNeg','IP2DFlip', 'IP3DFlip']
    if flags.BTagging.RunFlipTaggers:
        return base + flip
    return base


def createBTaggingConfigFlags():
    btagcf = AthConfigFlags()

    btagcf.addFlag("BTagging.taggerList", getTaggerList)
    btagcf.addFlag("BTagging.databaseScheme", "")
    btagcf.addFlag("BTagging.calibrationChannelAliases",
                   calibrationChannelAliases)
    btagcf.addFlag("BTagging.forcedCalibrationChannel", "")
    # the track association minimum is set to 4 GeV because of track
    # jets in offline reconstruction.
    btagcf.addFlag("BTagging.minimumJetPtForTrackAssociation", 4e3)
    # Disable JetVertexCharge ATLASRECTS-4506
    btagcf.addFlag("BTagging.RunModus", "analysis") # reference mode used in FlavourTagPerformanceFramework (RetagFragment.py)
    btagcf.addFlag("BTagging.ReferenceType", "ALL") # reference type for IP and SV taggers (B, UDSG, ALL)
    btagcf.addFlag("BTagging.JetPtMinRef", 15e3) # in MeV for uncalibrated pt
    btagcf.addFlag("BTagging.PrimaryVertexCollectionName", "PrimaryVertices")
    btagcf.addFlag("BTagging.Grades", getGrades)

    # Taggers for validation
    btagcf.addFlag("BTagging.SaveSV1Probabilities",False)
    btagcf.addFlag("BTagging.RunJetFitterNN",False)
    #Do we really need this in AthConfigFlags?
    #Comments in BTaggingConfiguration.py
    btagcf.addFlag("BTagging.OutputFiles.Prefix", "BTagging_")
    btagcf.addFlag("BTagging.GeneralToolSuffix",'') #Not sure it will stay like that later on. Was '', 'Trig, or 'AODFix'
    # Run the flip taggers
    btagcf.addFlag("BTagging.RunFlipTaggers", False)

    # experimental flags
    btagcf.addFlag("BTagging.Trackless", False)
    btagcf.addFlag("BTagging.Pseudotrack", False)

    return btagcf
