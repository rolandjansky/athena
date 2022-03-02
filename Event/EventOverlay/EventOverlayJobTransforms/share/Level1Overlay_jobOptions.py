# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
include.block("EventOverlayJobTransforms/Level1Overlay_jobOptions.py")

from AthenaCommon import CfgGetter
from AthenaCommon.DetFlags import DetFlags
from Digitization.DigitizationFlags import digitizationFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags


if DetFlags.overlay.LVL1_on():

    if overlayFlags.isDataOverlay():
        include("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
        #include ("MuonCommRecExample/ReadMuCTPI_jobOptions.py")

        ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
            "MuCTPI_RDO/MUCTPI_RDO",
            "L2Result/L2Result",
            "CTP_RDO/CTP_RDO",
            "MuCTPI_RIO/MUCTPI_RIO",
            "CTP_RIO/CTP_RIO"
        ]

    if DetFlags.simulateLVL1.LAr_on():
        include("LArL1Sim/LArL1Sim_G4_jobOptions.py")
        # Noise
        if not digitizationFlags.doCaloNoise.get_Value():
            job.LArTTL1Maker.NoiseOnOff = False  # (default:True)
        # PileUp
        job.LArTTL1Maker.PileUp = True
        # If we are doing MC overlay
        if not overlayFlags.isDataOverlay():
            if overlayFlags.isOverlayMT():
                job.LArTTL1Maker.EmTTL1ContainerName = overlayFlags.sigPrefix() + "LArTTL1EM"
                job.LArTTL1Maker.HadTTL1ContainerName = overlayFlags.sigPrefix() + "LArTTL1HAD"
            else:
                job.LArTTL1Maker.EmTTL1ContainerName = overlayFlags.evtStore() + "+LArTTL1EM"
                job.LArTTL1Maker.HadTTL1ContainerName = overlayFlags.evtStore() + "+LArTTL1HAD"

            job += CfgGetter.getAlgorithm("LArTTL1Overlay")
    # new L1Calo overlaying
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    if commonGeoFlags.Run()=="RUN3":
        from LArL1Sim.LArSCL1Getter import *
        theLArSCL1Getter = LArSCL1Getter()
        theLArSCL1Getter.BkgDigitKey="Bkg_LArDigitSCL2"
        #from LArROD.LArSCellGetter import LArSCellGetter
        #theLArSCellGetter = LArSCellGetter()

    if DetFlags.simulateLVL1.Tile_on():
        include("TileSimAlgs/TileTTL1_jobOptions.py")

        # If we are doing MC overlay
        if not overlayFlags.isDataOverlay():
            if overlayFlags.isOverlayMT():
                job.TileHitToTTL1.TileTTL1Container = overlayFlags.sigPrefix() + 'TileTTL1Cnt'
                job.TileHitToTTL1.TileMBTSTTL1Container = overlayFlags.sigPrefix() + \
                    'TileTTL1MBTS'
            else:
                job.TileHitToTTL1.TileTTL1Container = overlayFlags.evtStore() + '+TileTTL1Cnt'
                job.TileHitToTTL1.TileMBTSTTL1Container = overlayFlags.evtStore() + \
                    '+TileTTL1MBTS'

        include("TileSimAlgs/TileMuonReceiver_jobOptions.py")

        # Add special TTL1 overlay algorithm only for MC+MC overlay
        if not overlayFlags.isDataOverlay():
            job += CfgGetter.getAlgorithm("TileTTL1Overlay")


    if DetFlags.digitize.LVL1_on():
        from AthenaCommon.Logging import logging
        log = logging.getLogger("Level1Overlay_jobOptions")
        log.error("Running the LVL1 simulation during digitization is currently not supported.")
