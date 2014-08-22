## copied from InDetRecD3PDCreation.py
## Edward.Moyse@cern.ch

import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from MuonRecExample.MuonRecFlags import muonRecFlags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
from RecExConfig.RecFlags import rec
from MuonD3PDMaker.MuonD3PDMakerFlags    import MuonD3PDFlags

from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags

## D3PD Maker alg
MuonTrackD3PDMaker = D3PDMakerCoreComps.MakerAlg('MuonTrackTree',
topSequence,
'MuonTrackD3PD.root')

## Add blocks to the tree
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject

levelOfDetail = 2

if muonRecFlags.doTrkD3PD():

  MuonD3PDFlags.doSegmentTruth.set_Value_and_Lock(rec.doTruth())
    
  from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
  TrackD3PDFlags.doTruth = rec.doTruth()
  TrackD3PDFlags.storeTRTHitsOnTrack = False
  TrackD3PDFlags.storePixelHitsOnTrack = False
  TrackD3PDFlags.storeSCTHitsOnTrack = False
  TrackD3PDFlags.storeBLayerHitsOnTrack = False
  TrackD3PDFlags.storeCSCHitsOnTrack = True
  TrackD3PDFlags.storeMDTHitsOnTrack = True
  TrackD3PDFlags.storeRPCHitsOnTrack = True
  TrackD3PDFlags.storeTGCHitsOnTrack = True
  TrackD3PDFlags.storePullsAndResiduals = True
  ## Disable some things that are on by default
  TrackD3PDFlags.storeVertexFitQuality = False
  TrackD3PDFlags.storeVertexKinematics = False
  TrackD3PDFlags.storeVertexPurity = False
  TrackD3PDFlags.storeVertexTrackAssociation = False
  TrackD3PDFlags.storeVertexTrackIndexAssociation = False
  TrackD3PDFlags.storeTrackPredictionAtBLayer = False
  TrackD3PDFlags.storeVertexType = False
  TrackD3PDFlags.storeDetailedTruth = True
  
  #print TrackD3PDFlags
  
  from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
  from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject

  if (levelOfDetail>10):
    MuonTrackD3PDObject =   TrackD3PDObject(_label='muon',
                                            _prefix='mu_',
                                            _sgkey='MuonSpectrometerTracks',
                                            typeName='TrackCollection',  
                                            truthTarget='mc',
                                            truthPrefix='mc_',
                                            detailedTruthPrefix='detailed_mc_',
                                            truthMapKey='MuonSpectrometerTrackTruthCollection',
                                            detailedTruthMapKey='DetailedTrackTruth',
                                            SGKeyForDetailedTruth='MuonSpectrometerTracks',
                                            flags=TrackD3PDFlags)
    

    MuonTrackD3PDMaker += MuonTrackD3PDObject(levelOfDetail)

  if rec.doTruth():
    MuonTrackD3PDMaker += TruthTrackD3PDObject(levelOfDetail)


  MuonTrackPartD3PDObject =   TrackD3PDObject(_label='muTP',
                                              _prefix='muTP_',
                                              _sgkey='MuonSpectrometerParticles',
                                              typeName='Rec::TrackParticleContainer',
                                              truthTarget='mc',
                                              truthPrefix='mc_',
                                              detailedTruthPrefix='detailed_mc_',
                                              truthMapKey='MuonSpectrometerParticlesTruth',
                                              detailedTruthMapKey='MuonSpectrometerTracksTruth',
                                              flags=TrackD3PDFlags)
  MuonTrackD3PDMaker += MuonTrackPartD3PDObject(levelOfDetail)
    
  # TODO - add segments?
  from MuonD3PDMaker.MuonSegmentD3PDObject               import MuonSegmentD3PDObject
  from MuonD3PDMaker.MuonSegmentTruthD3PDObject          import MuonSegmentTruthD3PDObject
  MuonTrackD3PDMaker += MuonSegmentD3PDObject( 10, name = 'MuonSeg',
                                               sgkey='MuonSegments',
                                               prefix='muonseg_',
                                               include = ['MuonSegmentT0'],
                                               exclude = ['MuonboySegmentT0'],
                                               allowMissing=True
                                               )
  if muonRecFlags.doStandalone() and muonRecFlags.doNSWNewThirdChain() and muonStandaloneFlags.segmentOrigin == 'TruthTracking':
    MuonTrackD3PDMaker += MuonSegmentD3PDObject( 10, name = 'MuonSeg2',
                                                 sgkey='ThirdChainSegments',
                                                 prefix='muonseg2_',
                                                 include = ['MuonSegmentT0'],
                                                 exclude = ['MuonboySegmentT0'],
                                                 allowMissing=True
                                                 )
    
  # Truth    
  if rec.doTruth():
      from MuonD3PDMaker.TrackRecordD3PDObject            import TrackRecordD3PDObject
      MuonTrackD3PDMaker += TrackRecordD3PDObject(10, "TrackRecord")

