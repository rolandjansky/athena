# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PhotonD3PDMaker
import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from PhotonD3PDMaker import PhotonD3PDMakerConf
from TrackD3PDMaker import TrackD3PDMakerConf
from TrackD3PDMaker.TrackD3PDMakerFlags import *
from RecExConfig.RecFlags import rec

def DefineVertexD3PDObject(object,
                           trackTarget='trk',
                           trackPrefix='trk_',
                           trackType='Rec::TrackParticleContainer',
                           flags=TrackD3PDFlags):

    if trackType=='Rec::TrackParticleContainer':
        vxTrackAtVertexTrackAssociationTool = TrackD3PDMaker.VxTrackAtVertexTrackParticleAssociationTool
    if trackType=='TrackCollection':
        vxTrackAtVertexTrackAssociationTool = TrackD3PDMaker.VxTrackAtVertexTrkTrackAssociationTool

    # Kine
    object.defineBlock (0, 'Kine', PhotonD3PDMaker.PhotonPVFillerTool)


def BuildVertexD3PDObject(_prefix='vx_',
                     _label='vx',
                     _sgkey=D3PDMakerFlags.VertexSGKey(),
                     trackTarget='trk',
                     trackPrefix='trk_',
                     trackType='Rec::TrackParticleContainer'):

    object = make_SGDataVector_D3PDObject ('VxContainer',
                                           _sgkey,
                                           _prefix,
                                           default_allowMissing = True)
                                           
    DefineVertexD3PDObject(object,
                           trackTarget,
                           trackPrefix,
                           trackType)

    return object

PVD3PDObject = BuildVertexD3PDObject(_prefix='vx_',
                                     _label='vx',
                                     _sgkey='VxPrimaryCandidate',
                                     trackTarget='trk',
                                     trackPrefix='trk_',
                                     trackType='Rec::TrackParticleContainer')
