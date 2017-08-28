# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagTrackD3PDObject.py
## @brief track D3PD object configured for the btagging D3PD
## @author Georges Aad
## @date Nov, 2010
##
import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject
from RecExConfig.RecFlags import rec

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys

## configure TrackD3PDFlags before calling this function
##eg:
# TrackD3PDFlags.storeTrackMomentum = True
# TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 3
# TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 2
# TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 2
# TrackD3PDFlags.storeTrackFitQuality = True
# TrackD3PDFlags.storeTrackSummary = True
# TrackD3PDFlags.storeTrackInfo = True
# TrackD3PDFlags.storeTrackPredictionAtBLayer = True
# TrackD3PDFlags.storePixelHitsOnTrack = True
# TrackD3PDFlags.storeSCTHitsOnTrack = True
# TrackD3PDFlags.storeTRTHitsOnTrack = True
# TrackD3PDFlags.storePixelOutliersOnTrack = False
# TrackD3PDFlags.storeSCTOutliersOnTrack = False
# TrackD3PDFlags.storeTRTOutliersOnTrack = False
# TrackD3PDFlags.storePixelHolesOnTrack = False
# TrackD3PDFlags.storeSCTHolesOnTrack = False
# TrackD3PDFlags.storeTRTHolesOnTrack = False
# TrackD3PDFlags.doTruth = True
# TrackD3PDFlags.storeHitTruthMatching = True
# TrackD3PDFlags.storeDetailedTruth = False

def getJetTagTrackD3PDObject(level=20, **kw):

    if not rec.doTruth:
        TrackD3PDFlags.doTruth = False

    if TruthD3PDFlags.GenParticleAssocLabel() == None or TruthD3PDFlags.GenParticleAssocLabel() == "":
        TrackD3PDFlags.doTruth = False
        
 
    JetTagTrackParticleD3PDObject = TrackD3PDObject(_label=JetTagD3PDKeys.TrackGetterLabel(),
                                                    _prefix=JetTagD3PDKeys.TrackPrefix(),
                                                    _sgkey=JetTagD3PDFlags.TrackParticleSGKey(),
                                                    typeName='Rec::TrackParticleContainer',
                                                    vertexSGKey=JetTagD3PDFlags.PrimaryVertexSGKey(),
                                                    vertexTarget=JetTagD3PDKeys.PrimaryVertexGetterLabel(),
                                                    vertexPrefix=JetTagD3PDKeys.PrimaryVertexPrefix(),
                                                    truthTarget=TruthD3PDFlags.GenParticleAssocLabel(),
                                                    truthPrefix=TruthD3PDKeys.GenParticlePrefix(),
                                                    truthMapKey=JetTagD3PDFlags.TruthTrackParticleSGKey(),
                                                    detailedTruthMapKey='',#'DetailedTrackTruth',
                                                    SGKeyForTruth=JetTagD3PDFlags.TrackParticleSGKey(),
                                                    flags=TrackD3PDFlags)

    from AthenaCommon.AppMgr import ToolSvc
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrackSelectorTool
    jetTagD3PDInDetTrackSelectorTool = InDet__InDetTrackSelectorTool("JetTagD3PDInDetTrackSelectorTool")
    jetTagD3PDInDetTrackSelectorTool.minPt = JetTagD3PDFlags.TrackPtCut()
    jetTagD3PDInDetTrackSelectorTool.IPz0Max = 99999.
    jetTagD3PDInDetTrackSelectorTool.maxZ0 = 99999.
    jetTagD3PDInDetTrackSelectorTool.maxD0 = 99999.
    jetTagD3PDInDetTrackSelectorTool.maxD0overSigmaD0 = 99999.
    jetTagD3PDInDetTrackSelectorTool.numberOfPixelHits = -99
    jetTagD3PDInDetTrackSelectorTool.numberOfInnermostPixelLayerHits = -99
    jetTagD3PDInDetTrackSelectorTool.Extrapolator = ToolSvc.InDetExtrapolator 
    jetTagD3PDInDetTrackSelectorTool.TrackSummaryTool = ToolSvc.InDetTrackSummaryTool
    ToolSvc+=jetTagD3PDInDetTrackSelectorTool

    from TrackD3PDMaker import TrackParticleGetterTool
    jetTagD3PDTrackGetterTool = TrackParticleGetterTool("jetTagD3PDTrackGetterTool")
    jetTagD3PDTrackGetterTool.VertexSGKey = JetTagD3PDFlags.PrimaryVertexSGKey() ## no need to resolve for now 
    jetTagD3PDTrackGetterTool.TrackSelector = jetTagD3PDInDetTrackSelectorTool
    jetTagD3PDTrackGetterTool.Label = JetTagD3PDKeys.TrackGetterLabel()
    jetTagD3PDTrackGetterTool.SGKey = JetTagD3PDFlags.TrackParticleSGKey()
    jetTagD3PDTrackGetterTool.TypeName = 'Rec::TrackParticleContainer'

    
    return JetTagTrackParticleD3PDObject(level=level,
                                         getter=jetTagD3PDTrackGetterTool, **kw)






