import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from InDetRecExample.InDetKeys import InDetKeys
#from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags

## D3PD Maker alg
InDetTrackD3PDMaker = D3PDMakerCoreComps.MakerAlg('InDetTrackTree',
                                                   topSequence,
                                                   InDetKeys.trkD3PDFileName())

## Add blocks to the tree
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
InDetTrackD3PDMaker += EventInfoD3PDObject(0, prefix='ei_')
levelOfDetail = 8
if InDetFlags.doTruth():
   levelOfDetail = 10
   from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
   InDetTrackD3PDMaker += TruthTrackD3PDObject(levelOfDetail)
   from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
   InDetTrackD3PDMaker += TruthVertexD3PDObject(levelOfDetail)
if InDetFlags.doTriggerD3PD():
   from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
   cfg = TriggerConfigGetter("ReadPool")
   from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
   from TriggerD3PDMaker.TrigConfMetadata  import addTrigConfMetadata
   from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
   InDetTrackD3PDMaker += TrigDecisionD3PDObject(0)
   addTrigConfMetadata(InDetTrackD3PDMaker)
if InDetFlags.doTrkD3PD():
   from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
   InDetTrackD3PDMaker += TrackParticleD3PDObject(levelOfDetail)
if InDetFlags.doVtxD3PD():
   from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
   InDetTrackD3PDMaker += PrimaryVertexD3PDObject(levelOfDetail)
if InDetFlags.doV0VtxD3PD():
   from TrackD3PDMaker.V0D3PDObject import V0D3PDObject
   InDetTrackD3PDMaker += V0D3PDObject(10)
if InDetFlags.doPixelTrkD3PD():
   from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
   InDetTrackD3PDMaker += PixelTrackD3PDObject(levelOfDetail)
if InDetFlags.doSctTrkD3PD():
   from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
   InDetTrackD3PDMaker += SCTTrackD3PDObject(levelOfDetail)
if InDetFlags.doTrtTrkD3PD():
   from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
   InDetTrackD3PDMaker += TRTTrackD3PDObject(levelOfDetail)
if InDetFlags.doVtxMonitoringD3PD():

  from TrackD3PDMaker.VertexD3PDObject import BuildVertexD3PDObject
  BeamConstraintPrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='vxbc_',
                                                  _label='vxbc',
                                                  _sgkey=InDetKeys.PrimaryVertices(),
                                                  trackTarget='trk',
                                                  trackPrefix='trk_',
                                                  trackType='Rec::TrackParticleContainer')
  InDetTrackD3PDMaker += BeamConstraintPrimaryVertexD3PDObject(8)

  SplitPrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='vxsplit_',
                                                  _label='vxsplit',
                                                  _sgkey=InDetKeys.PrimaryVerticesSplitStream(),
                                                  trackTarget='trk',
                                                  trackPrefix='trk_',
                                                  trackType='Rec::TrackParticleContainer')
  InDetTrackD3PDMaker += SplitPrimaryVertexD3PDObject(8)

  NoBeamConstraintPrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='vxnbc_',
                                                  _label='vxnbc',
                                                  _sgkey=InDetKeys.PrimaryVerticesWithoutBeamConstraint(),
                                                  trackTarget='trk',
                                                  trackPrefix='trk_',
                                                  trackType='Rec::TrackParticleContainer')
  InDetTrackD3PDMaker += NoBeamConstraintPrimaryVertexD3PDObject(8)

# WORKAROUND: only uncomment these lines if you read RAW data BS
# and you do not produce an output pool file (e.g. ESD or AOD)
# The string AANT1 will change to D3PD in the future
#from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
#AANTupleStream('AANT1').ExistDataHeader = False
