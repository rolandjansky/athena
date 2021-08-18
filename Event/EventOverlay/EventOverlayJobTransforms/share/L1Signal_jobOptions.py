
include.block ( "EventOverlayJobTransforms/L1Signal_jobOptions.py" )

from AthenaCommon.Logging import logging

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
from Digitization.DigitizationFlags import jobproperties

if DetFlags.overlay.LVL1_on():

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    if DetFlags.simulateLVL1.LAr_on():
        from LArL1Sim.LArL1SimConf import *
        newLArTTL1Maker=LArTTL1Maker("newLArTTL1Maker")
        # check if LArdigitization is run before. If yes, uses hit map from detector store produces from lardigitization
        if not hasattr(topSequence,"digitmaker2"):
           newLArTTL1Maker.useMapFromStore = False

        job += newLArTTL1Maker
        # Noise 
        if not jobproperties.Digitization.doCaloNoise.get_Value():
            job.newLArTTL1Maker.NoiseOnOff= False #(default:True) 
        # PileUp
        job.newLArTTL1Maker.PileUp = True
        job.newLArTTL1Maker.EventStore = "BkgEvent_0_SG"    

    if DetFlags.simulateLVL1.Tile_on():
       from TileSimAlgs.TileSimAlgsConf import TileHitToTTL1
       newTileHitToTTL1=TileHitToTTL1("newTileHitToTTL1")
       newTileHitToTTL1.TileHitContainer=job.TileHitToTTL1.TileHitContainer
       newTileHitToTTL1.TileInfoName=job.TileHitToTTL1.TileInfoName
       newTileHitToTTL1.TileTTL1Container=job.TileHitToTTL1.TileTTL1Container
       newTileHitToTTL1.EvtStore = "BkgEvent_0_SG" 
       job += newTileHitToTTL1

       from Digitization.DigitizationFlags import jobproperties
       newTileHitToTTL1.RndmSvc=jobproperties.Digitization.rndmSvc()
       jobproperties.Digitization.rndmSeedList.addSeed("newTile_HitToTTL1", 2789801, 189245514)

    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == 'cosmics':
         newTileTTL1Cosmics=TileHitToTTL1("newTileTTL1Cosmics")
         newTileTTL1Cosmics.RndmSvc=jobproperties.Digitization.rndmSvc()
         jobproperties.Digitization.rndmSeedList.addSeed("newTile_TTL1Cosmics", 1789891, 189240513)
         newTileTTL1Cosmics.EvtStore = "BkgEvent_0_SG"
         job += newTileTTL1Cosmics 
         job.newTileTTL1Cosmics.TileTTL1Type = job.TileHitToTTL1_Cosmics.TileTTL1Type 
         job.newTileTTL1Cosmics.TileHitContainer = job.TileHitToTTL1_Cosmics.TileHitContainer
         job.newTileTTL1Cosmics.TileTTL1Container = job.TileHitToTTL1_Cosmics.TileTTL1Container
         job.newTileTTL1Cosmics.TileMBTSTTL1Container = job.TileHitToTTL1_Cosmics.TileMBTSTTL1Container    

    if DetFlags.digitize.LVL1_on():

       #--------------------------------------------------------------
       # RPC stuff
       #--------------------------------------------------------------
       if DetFlags.simulateLVL1.RPC_on():
          from TrigT1RPCsteering.TrigT1RPCsteeringConf import *
          newTrigT1RPC = TrigT1RPC("newTrigT1RPC")
          newTrigT1RPC.EventStore = "BkgEvent_0_SG"
          job += newTrigT1RPC

       #--------------------------------------------------------------
       # TGC stuff
       #--------------------------------------------------------------
       if DetFlags.simulateLVL1.TGC_on():
          from TrigT1TGC.TrigT1TGCConf import LVL1TGCTrigger__LVL1TGCTrigger
          newTrigT1TGC = LVL1TGCTrigger__LVL1TGCTrigger ("newTrigT1TGC")
          newTrigT1TGC.EventStore = "BkgEvent_0_SG"
          job += newTrigT1TGC
          job.newTrigT1TGC.InputData_perEvent = "TGC_DIGITS"
          job.newTrigT1TGC.MuCTPIInput_TGC = "L1MuctpiStoreTGC"
          job.newTrigT1TGC.MaskFileName12 = "TrigT1TGCMaskedChannel._12.db"

       #--------------------------------------------------------------
       # TrigT1Muctpi Algos
       #--------------------------------------------------------------
       if DetFlags.simulateLVL1.RPC_on() or DetFlags.simulateLVL1.TGC_on():
          from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
          #ACH job += L1Muctpi("newL1Muctpi")
          #ACH job.newL1Muctpi.StoreGateSvc = "BkgEvent_0_SG"
       #-------------------------------------------------------
       # TrigT1MBTS Alg
       #-------------------------------------------------------
       if DetFlags.simulateLVL1.Calo_on():
          from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
          job += LVL1__TrigT1MBTS("newTrigT1MBTS")
          job.newTrigT1MBTS.EvtStore = "BkgEvent_0_SG"

       #-------------------------------------------------------
       # TrigT1BCM Alg
       #-------------------------------------------------------
       if DetFlags.simulateLVL1.BCM_on():
          from TrigT1BCM.TrigT1BCMConf import LVL1__TrigT1BCM
          job += LVL1__TrigT1BCM("newTrigT1BCM")
          job.newTrigT1BCM.EvtStore = "BkgEvent_0_SG"

       #-------------------------------------------------------
       # TrigT1CTP Algos
       #-------------------------------------------------------
       from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInOverlay
       job += CTPSimulationInOverlay("newCTPSimulationInDigi","CTPSimulation2")
       job.newCTPSimulationInDigi.EvtStore = "BkgEvent_0_SG"

       #-------------------------------------------------------
       # TrigT1RoIB Algos
       #-------------------------------------------------------
       from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilderInDigi
       job += RoIBuilderInDigi("newRoIBuilder")
       job.newRoIBuilder.StoreGateSvc = "BkgEvent_0_SG"
    
