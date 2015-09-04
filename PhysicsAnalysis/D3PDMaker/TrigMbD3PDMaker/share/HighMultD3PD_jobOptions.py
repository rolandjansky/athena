
DoReconstruction = False 
DoTruth = True 
DoTracklets = False 
DoTrigger = True 
DoJets = True
DoTrigCalo = True
DoLucid = True

if not DoReconstruction:
   DoTracklets = False

from TrigMbD3PDMaker.TrigMbD3PDMakerFlags import trigMbD3PDflags
#from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon.Logging import logging

log = logging.getLogger("HighMultD3PD_jobOptions.py")

log.info("inputFileSummary['evt_type'] = " + str(inputFileSummary['evt_type']))

if 'IS_DATA' in inputFileSummary['evt_type']:
  DoTruth = False

outFileName = trigMbD3PDflags.FileName()

import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TrigMbD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

## Top Sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if not "TrigMbD3PDMaker" in theApp.Dlls:
  theApp.Dlls += ["TrigMbD3PDMaker"]

## Is this preprocessing necessary?
## Preprocessing
from MinBiasD3PDMaker.MinBiasD3PDMakerConf import MinBiasD3PD__MinBiasD3PDPreProcessing
MinBiasD3PDPreProcessingAlg = MinBiasD3PD__MinBiasD3PDPreProcessing( name = "MinBiasD3PDPreProcessingAlg",
                                                                     OutputLevel = INFO )
topSequence += MinBiasD3PDPreProcessingAlg

# TrackD3PDMaker Flags
from TrackD3PDMaker.TrackD3PDMakerFlags import *
TrackD3PDFlags.storeHitTruthMatching = True
TrackD3PDFlags.storeDetailedTruth = True
TrackD3PDFlags.storePixelHitsOnTrack = False
TrackD3PDFlags.storeSCTHitsOnTrack = False
TrackD3PDFlags.storeTRTHitsOnTrack = False
TrackD3PDFlags.storePixelOutliersOnTrack = False
TrackD3PDFlags.storeSCTOutliersOnTrack = False
TrackD3PDFlags.storeTRTOutliersOnTrack = False
TrackD3PDFlags.storePixelHolesOnTrack = False
TrackD3PDFlags.storeSCTHolesOnTrack = False
TrackD3PDFlags.storeTRTHolesOnTrack = False
TrackD3PDFlags.storeTrackPredictionAtBLayer = True
TrackD3PDFlags.storeTrackInfo = True
TrackD3PDFlags.storeTrackFitQuality = True
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 1
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 1
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 1
TrackD3PDFlags.storeTrackMomentum = True
TrackD3PDFlags.vertexPositionLevelOfDetails = 1
TrackD3PDFlags.storeVertexTrackAssociation = True
TrackD3PDFlags.storeVertexTrackIndexAssociation = True
TrackD3PDFlags.doTruth = DoTruth

## Event Info
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
#EventInfoD3PDObject = D3PDObject (makeEventInfoD3PDObject, 'ei_')
#EventInfoD3PDObject.defineBlock (0, 'eventID', EventCommonD3PDMaker.EventIDFillerTool)

## Beam Spot
from MinBiasD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject
## BCM RDO
from TrigMbD3PDMaker.BcmRdoD3PDObject import BcmRdoD3PDObject
## CTP RDO
from TrigMbD3PDMaker.CtpRdoD3PDObject import CtpRdoD3PDObject
## CTP Decision
from TrigMbD3PDMaker.CtpDecisionD3PDObject import CtpDecisionD3PDObject
## Lucid Raw info
if DoLucid:
  from TrigMbD3PDMaker.LucidRawD3PDObject import LucidRawD3PDObject
## MBTS Tile Cells
from TrigMbD3PDMaker.MbtsContainerD3PDObject import MbtsContainerD3PDObject
## MBTS LVL2
from TrigMbD3PDMaker.MbtsLvl2D3PDObject import MbtsLvl2D3PDObject
## SP LVL2
from TrigMbD3PDMaker.SpLvl2D3PDObject import SpLvl2D3PDObject
## TRT LVL2
from TrigMbD3PDMaker.TrtLvl2D3PDObject import TrtLvl2D3PDObject
## Trigger Track Counts
from TrigMbD3PDMaker.TrkCntsEfD3PDObject import TrkCntsEfD3PDObject

from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
## Trigger meta data
from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata

## Tracks
from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
from TrackD3PDMaker.V0D3PDObject import DefaultV0D3PDObject as V0D3PDObject
from TrackD3PDMaker.PixelClusterD3PDObject import PixelClusterD3PDObject

## Calo Info
from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject

## Jets
from JetD3PDMaker.JetD3PDObject  import JetD3PDObject

## D3PD Maker alg
alg = D3PDMakerCoreComps.MakerAlg('MinBiasTree',
                                  topSequence,
                                  outFileName)

alg += EventInfoD3PDObject(10)
alg += BeamSpotD3PDObject(10)

if DoTrigger:
   alg += CtpRdoD3PDObject(10)
   alg += CtpDecisionD3PDObject(10)
   alg += MbtsContainerD3PDObject(10)
   alg += SpLvl2D3PDObject(10)
   alg += TrtLvl2D3PDObject(10)
   alg += TrkCntsEfD3PDObject(10)
   alg += TrigDecisionD3PDObject(1)
   addTrigConfMetadata(alg)

if DoTruth:
   alg += TruthVertexD3PDObject(10)
   alg += TruthTrackD3PDObject(10)

alg += PrimaryVertexD3PDObject(10)
alg += TrackParticleD3PDObject(10)

if DoTracklets:
   alg += PixelTrackD3PDObject(10)
   alg += SCTTrackD3PDObject(10)
   alg += TRTTrackD3PDObject(10)

if DoTrigCalo:
  alg += EnergySumROID3PDObject(level = 2,prefix="trig_L1_esum_" )
  alg += TrigMETD3PDObject( level = 1 , prefix = "trig_EF_", sgkey = "HLT_TrigEFMissingET" )
  alg += TrigMETD3PDObject( level = 1, prefix = "trig_L2_", sgkey = "HLT_T2MissingET" )

if DoLucid:
  alg += LucidRawD3PDObject(10)

if DoJets:
  alg += JetD3PDObject (level=1, sgkey='AntiKt6TopoJets',  prefix='AntiKt6H1Topo_', include=['AssocTrackCont']) 
  alg += JetD3PDObject (level=1, sgkey='AntiKt4TopoJets',  prefix='AntiKt4H1Topo_', include=['AssocTrackCont'])






