# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import PhotonD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TruthD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from AthenaCommon.AppMgr import ToolSvc

def PhotonTrackD3PDObject(_label='trkPhotonTrack',
                          _prefix='trkPhotonTrack_',
                          _sgkey='Tracks',
                          _object_name='PhotonTrackD3PDObject',
                          typeName='TrackCollection'):
    
    object = make_SGDataVector_D3PDObject (typeName,
                                           _sgkey,
                                           _prefix,
                                           _object_name,
                                           default_allowMissing = True,
                                           default_label = _label,
                                           allow_args = ['GenParticleTarget',
                                                         'TruthParticleTarget',
                                                         'StoreTruthInfo'])
        
    if typeName=='Rec::TrackParticleContainer':
        ParametersAtBLFillerTool     = PhotonD3PDMaker.TrackParticleParametersAtDiPhotonVertex

    object.defineBlock(0, _prefix+"ParametersAtBL",
                       ParametersAtBLFillerTool)

    return object



PhotonTrackParticleD3PDObject = PhotonTrackD3PDObject(_label='trkPho',
                                                      _prefix='trkPho_',
                                                      _sgkey=D3PDMakerFlags.TrackSGKey(),
                                                      _object_name='PhotonTrackParticleD3PDObject',
                                                      typeName='Rec::TrackParticleContainer')
