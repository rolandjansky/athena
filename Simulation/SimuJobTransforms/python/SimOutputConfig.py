# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def getStreamEVNT_TR_ItemList(ConfigFlags):
    #Add to item list
    ItemList = [
        "IOVMetaDataContainer#*",
        "EventInfo#*"
    ]
    if ConfigFlags.Sim.CavernBG in ['Write', 'WriteWorld']:
        ItemList += ["TrackRecordCollection#NeutronBG"]
    else:
        ItemList += ["TrackRecordCollection#CosmicRecord"]
    return ItemList


def getStreamHITS_ItemList(ConfigFlags):
    #Add to item list
    #TODO - make a separate function (combine with G4AtlasAlg one?)
    ItemList = ["EventInfo#*",
                "McEventCollection#TruthEvent",
                "JetCollection#*"]

    if ConfigFlags.Sim.IncludeParentsInG4Event:
        ItemList += ["McEventCollection#GEN_EVENT"]

    ItemList += ["xAOD::JetContainer#*",
                 "xAOD::JetAuxContainer#*"]

    # pile-up truth particles
    ItemList += ["xAOD::TruthParticleContainer#TruthPileupParticles",
                 "xAOD::TruthParticleAuxContainer#TruthPileupParticlesAux."]

    if 'Hijing_event_params' in ConfigFlags.Input.Collections:
        ItemList += ["HijingEventParams#Hijing_event_params"]

    if ConfigFlags.Detector.EnablePixel or  ConfigFlags.Detector.EnableSCT or \
       ConfigFlags.Detector.EnableITkPixel or  ConfigFlags.Detector.EnableITkStrip or ConfigFlags.Detector.EnablePLR or \
       ConfigFlags.Detector.EnableHGTD:
        ItemList += ["SiHitCollection#*"]

    if ConfigFlags.Detector.EnableTRT:
        ItemList += ["TRTUncompressedHitCollection#*"]

    if ConfigFlags.Detector.EnableID or ConfigFlags.Detector.EnableITk:
       ItemList += ["TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.EnableCalo:
        ItemList += ["CaloCalibrationHitContainer#*",
                     "TrackRecordCollection#MuonEntryLayer"]

    if ConfigFlags.Detector.EnableLAr:
        ItemList += ["LArHitContainer#LArHitEMB",
                     "LArHitContainer#LArHitEMEC",
                     "LArHitContainer#LArHitHEC",
                     "LArHitContainer#LArHitFCAL"]
        if ConfigFlags.Sim.ISF.HITSMergingRequired.get('CALO', False):
            ItemList += ["LArHitContainer#LArHitEMB_G4",
                         "LArHitContainer#LArHitEMEC_G4",
                         "LArHitContainer#LArHitHEC_G4",
                         "LArHitContainer#LArHitFCAL_G4",
                         "LArHitContainer#LArHitEMB_FastCaloSim",
                         "LArHitContainer#LArHitEMEC_FastCaloSim",
                         "LArHitContainer#LArHitHEC_FastCaloSim",
                         "LArHitContainer#LArHitFCAL_FastCaloSim"]

    if ConfigFlags.Detector.EnableTile:
        ItemList += ["TileHitVector#TileHitVec",
                     "TileHitVector#MBTSHits"]
        if ConfigFlags.Sim.ISF.HITSMergingRequired.get('CALO', False):
            ItemList += ["TileHitVector#MBTSHits_G4",
                         "TileHitVector#TileHitVec_G4",
                         "TileHitVector#TileHitVec_FastCaloSim"]

    if ConfigFlags.Detector.EnableRPC:
        ItemList += ["RPCSimHitCollection#*"]

    if ConfigFlags.Detector.EnableTGC:
        ItemList += ["TGCSimHitCollection#*"]

    if ConfigFlags.Detector.EnableMDT:
        ItemList += ["MDTSimHitCollection#*"]

    if ConfigFlags.Detector.EnableCSC:
        ItemList += ["CSCSimHitCollection#*"]

    if ConfigFlags.Detector.EnablesTGC:
        ItemList += ["sTGCSimHitCollection#*"]

    if ConfigFlags.Detector.EnableMM:
        ItemList += ["MMSimHitCollection#*"]

    if ConfigFlags.Detector.EnableMuon:
        ItemList += ["TrackRecordCollection#MuonExitLayer"]

    if ConfigFlags.Detector.EnableLucid:
        ItemList += ["LUCID_SimHitCollection#*"]

    if ConfigFlags.Detector.EnableFwdRegion:
        ItemList += ["SimulationHitCollection#*"]

    if ConfigFlags.Detector.EnableZDC:
        ItemList += ["ZDC_SimPixelHit_Collection#*",
                     "ZDC_SimStripHit_Collection#*"]

    if ConfigFlags.Detector.EnableALFA:
        ItemList += ["ALFA_HitCollection#*",
                     "ALFA_ODHitCollection#*"]

    if ConfigFlags.Detector.EnableAFP:
        ItemList += ["AFP_TDSimHitCollection#*",
                     "AFP_SIDSimHitCollection#*"]

    from AthenaConfiguration.Enums import BeamType
    if ConfigFlags.Beam.Type is BeamType.Cosmics:
        ItemList += ["TrackRecordCollection#CosmicRecord",
                     "TrackRecordCollection#CosmicPerigee"]

    # TimingAlg
    ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]
    return ItemList
