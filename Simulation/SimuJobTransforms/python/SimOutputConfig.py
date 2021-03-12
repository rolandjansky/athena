# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


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

    if ConfigFlags.Detector.EnablePixel or  ConfigFlags.Detector.EnableSCT or \
       ConfigFlags.Detector.EnableITkPixel or  ConfigFlags.Detector.EnableITkStrip:
        ItemList += ["SiHitCollection#*"]

    if ConfigFlags.Detector.EnableTRT:
        ItemList += ["TRTUncompressedHitCollection#*"]

    if ConfigFlags.Detector.EnableID or ConfigFlags.Detector.EnableITk:
       ItemList += ["TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.EnableCalo:
        ItemList += ["CaloCalibrationHitContainer#*",
                     "TrackRecordCollection#MuonEntryLayer"]

    if ConfigFlags.Detector.EnableLAr:
        ItemList += ["LArHitContainer#*"]

    if ConfigFlags.Detector.EnableTile:
        ItemList += ["TileHitVector#*"]

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

    # TimingAlg
    ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]
    return ItemList
