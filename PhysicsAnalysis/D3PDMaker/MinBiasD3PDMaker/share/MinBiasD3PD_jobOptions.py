
# MinBias flags
from MinBiasD3PDMaker.MinBiasD3PDMakerFlags import minbiasD3PDflags

if minbiasD3PDflags.doPixelTracklets():
    print ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> TRACKLETS"

## Don't do truth on data
if globalflags.DataSource == 'data':
    minbiasD3PDflags.doMBTruth = False

#--------------------------------------------------------------
# Configure Beam spot service
#--------------------------------------------------------------

#from AthenaCommon.GlobalFlags import globalflags
#if globalflags.DataSource == 'data':
#    include("InDetBeamSpotService/BeamCondSvc.py")
#    conddb.addOverride("/Indet/Beampos", 
#minbiasD3PDflags.BSDBFolderName())

#--------------------------------------------------------------
# Configure the MinBiasD3PDMaker
#--------------------------------------------------------------

import TrackD3PDMaker
import TrigMbD3PDMaker
import TriggerD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

## Output file name
MinBiasD3PDFileName = minbiasD3PDflags.FileName()

## Top Sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Load MinBiasD3PDMaker 
if not "MinBiasD3PDMaker" in theApp.Dlls:
    theApp.Dlls += [ "MinBiasD3PDMaker" ]

## Event Info
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject

## Trigger part
from TrigMbD3PDMaker.BcmRdoD3PDObject import BcmRdoD3PDObject
from TrigMbD3PDMaker.CtpRdoD3PDObject import CtpRdoD3PDObject
from TrigMbD3PDMaker.CtpDecisionD3PDObject import CtpDecisionD3PDObject
from TrigMbD3PDMaker.MbtsContainerD3PDObject import MbtsContainerD3PDObject
from TrigMbD3PDMaker.MbtsLvl2D3PDObject import MbtsLvl2D3PDObject
from TrigMbD3PDMaker.SpLvl2D3PDObject import SpLvl2D3PDObject
from TrigMbD3PDMaker.TrtLvl2D3PDObject import TrtLvl2D3PDObject
from TrigMbD3PDMaker.TrkCntsEfD3PDObject import TrkCntsEfD3PDObject
from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata

## Tracking part
#-- TrackD3PDMaker Flags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.doTruth.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
TrackD3PDFlags.storeDiagonalCovarianceAsErrors.set_Value_and_Lock(True)
TrackD3PDFlags.storeHitTruthMatching.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
TrackD3PDFlags.storeDetailedTruth.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
TrackD3PDFlags.storePullsAndResiduals.set_Value_and_Lock(False)
TrackD3PDFlags.storeBLayerHitsOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storePixelHitsOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeSCTHitsOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeTRTHitsOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeBLayerOutliersOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storePixelOutliersOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeSCTOutliersOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeTRTOutliersOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeBLayerHolesOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storePixelHolesOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeSCTHolesOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeTRTHolesOnTrack.set_Value_and_Lock(True)
TrackD3PDFlags.storeVertexAssociation.set_Value_and_Lock(False)
TrackD3PDFlags.storeTrackPredictionAtBLayer.set_Value_and_Lock(True)
TrackD3PDFlags.storeTrackInfo.set_Value_and_Lock(True)
TrackD3PDFlags.storeTrackFitQuality.set_Value_and_Lock(True)
TrackD3PDFlags.storeTrackSummary.set_Value_and_Lock(True)
TrackD3PDFlags.storeTrackSummary.IDHits = True
TrackD3PDFlags.storeTrackSummary.IDHoles = True
TrackD3PDFlags.storeTrackSummary.IDSharedHits = True
TrackD3PDFlags.storeTrackSummary.IDOutliers = True
TrackD3PDFlags.storeTrackSummary.PixelInfoPlus = False
TrackD3PDFlags.storeTrackSummary.SCTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.TRTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.InfoPlus = False
TrackD3PDFlags.storeTrackSummary.MuonHits = False
TrackD3PDFlags.storeTrackSummary.MuonHoles = False
TrackD3PDFlags.storeTrackSummary.ExpectBLayer = True
TrackD3PDFlags.storeTrackSummary.HitSum = True
TrackD3PDFlags.storeTrackSummary.HoleSum = True
TrackD3PDFlags.storeTrackSummary.HitPattern = True
TrackD3PDFlags.storeTrackSummary.SiHits = False
TrackD3PDFlags.storeTrackSummary.TRTRatio = False
TrackD3PDFlags.storeTrackSummary.PixeldEdx = False
TrackD3PDFlags.storeTrackSummary.ElectronPID = False
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails.set_Value_and_Lock(3)
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails.set_Value_and_Lock(3)
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails.set_Value_and_Lock(3)
TrackD3PDFlags.storeTrackUnbiasedIPAtPV.set_Value_and_Lock(False)
TrackD3PDFlags.storeTrackMomentum.set_Value_and_Lock(True)
TrackD3PDFlags.vertexPositionLevelOfDetails.set_Value_and_Lock(3)
TrackD3PDFlags.storeVertexFitQuality.set_Value_and_Lock(True)
TrackD3PDFlags.storeVertexKinematics.set_Value_and_Lock(True)
TrackD3PDFlags.storeVertexPurity.set_Value_and_Lock(minbiasD3PDflags.doMBTruth())
TrackD3PDFlags.storeVertexTrackAssociation.set_Value_and_Lock(False)
TrackD3PDFlags.storeVertexTrackIndexAssociation.set_Value_and_Lock(True)
#-- TrackD3PDMaker configuration
from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject import ResolvedTracksD3PDObject
from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
from TrackD3PDMaker.VertexD3PDObject import BuildVertexD3PDObject
SecVertexD3PDObject = BuildVertexD3PDObject(_prefix='secVx_',
                                            _label='secVx',
                                            _sgkey='SecVertices',
                                            trackTarget='trk',
                                            trackPrefix='trk_',
                                            trackType='Rec::TrackParticleContainer')
from TrackD3PDMaker.V0D3PDObject import V0D3PDObject
from TrackD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject

## MinBias part
from MinBiasD3PDMaker.UnassociatedHitsD3PDObject import UnassociatedHitsD3PDObject

## D3PD Maker alg
alg = D3PDMakerCoreComps.MakerAlg('MinBiasTree',
                                  topSequence,
                                  MinBiasD3PDFileName)

## Add blocks to the tree
alg += EventInfoD3PDObject(10, prefix='ei_')
alg += TrackParticleD3PDObject(10)
alg += PrimaryVertexD3PDObject(10)
if minbiasD3PDflags.doBeamSpot():
    alg += BeamSpotD3PDObject(10)
if minbiasD3PDflags.doUnassociatedHits():
    alg += UnassociatedHitsD3PDObject(10)
if minbiasD3PDflags.doTrigger():
    alg += CtpRdoD3PDObject(10)
    alg += CtpDecisionD3PDObject(10)
    alg += MbtsContainerD3PDObject(10)
    alg += TrigDecisionD3PDObject(10)
    addTrigConfMetadata( alg )
if minbiasD3PDflags.doDetailedTrigger():
    alg += BcmRdoD3PDObject(10)
    alg += MbtsLvl2D3PDObject(10)
    alg += SpLvl2D3PDObject(10)
    alg += TrtLvl2D3PDObject(10)
    alg += TrkCntsEfD3PDObject(10)
if minbiasD3PDflags.doMBTruth():
    alg += TruthTrackD3PDObject(10)
    alg += TruthVertexD3PDObject(10)
if minbiasD3PDflags.doPixelTracklets():
    alg += PixelTrackD3PDObject(10)
if minbiasD3PDflags.doSCTTracklets():
    alg += SCTTrackD3PDObject(10)
if minbiasD3PDflags.doTRTTracklets():
    alg += TRTTrackD3PDObject(10)
if minbiasD3PDflags.doResolvedTracklets():
    alg += ResolvedTracksD3PDObject(10)
if minbiasD3PDflags.doV0s():
    alg += V0D3PDObject(10, sgkey="V0Candidates")
    alg += SecVertexD3PDObject(10)

if minbiasD3PDflags.doLucid():
    from TrigMbD3PDMaker.LucidRawD3PDObject import LucidRawD3PDObject
    alg += LucidRawD3PDObject(10)
    if globalflags.DataSource=='geant4':
        from TrigMbD3PDMaker.LucidDigitD3PDObject import LucidDigitD3PDObject
        d3pdalg += LucidDigitD3PDObject(10)

if minbiasD3PDflags.doZDC():
    ##ZDC object
    from ForwardDetectorsD3PDMaker.ZdcD3PDObject import ZdcD3PDObject
    from ForwardDetectorsD3PDMaker.ZdcDigitsD3PDObject import ZdcDigitsD3PDObject
    alg += ZdcD3PDObject(10)
    alg += ZdcDigitsD3PDObject(10)

from CaloD3PDMaker.MBTSTimeD3PDObject import MBTSTimeD3PDObject
alg += MBTSTimeD3PDObject (10)
