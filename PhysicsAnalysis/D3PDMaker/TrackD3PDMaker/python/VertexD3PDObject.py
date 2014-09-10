# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.flagTestLOD import flagTestLOD, deferFlag
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.resolveSGKey import resolveSGKey
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker import TrackD3PDMakerConf
from TrackD3PDMaker.TrackD3PDMakerFlags import *
from RecExConfig.RecFlags import rec


def _get_purity (args, hookargs):
    from AthenaCommon.AppMgr import ToolSvc
    from InDetRecExample.InDetKeys import InDetKeys
    from InDetRecExample.InDetJobProperties import InDetFlags
    # this tool is only needed for D3PD writing and hence it is configured here
    from TrkVertexFitterValidationUtils.TrkVertexFitterValidationUtilsConf import Trk__TrkPriVxPurityTool
    mckey = resolveSGKey ('McEventCollection', D3PDMakerFlags.TruthSGKey())
    TrkPriVxPurityTool = Trk__TrkPriVxPurityTool(MonteCarloCollection = mckey)
    ToolSvc += TrkPriVxPurityTool
    if InDetFlags.doPrintConfigurables():
        print TrkPriVxPurityTool
    return


def DefineVertexD3PDObject(object,
                           trackTarget='trk',
                           trackPrefix='trk_',
                           trackType='Rec::TrackParticleContainer',
                           flags=TrackD3PDFlags):

    if trackType=='Rec::TrackParticleContainer':
        vxTrackAtVertexTrackAssociationTool = TrackD3PDMaker.VxTrackAtVertexTrackParticleAssociationTool
    if trackType=='TrackCollection':
        vxTrackAtVertexTrackAssociationTool = TrackD3PDMaker.VxTrackAtVertexTrkTrackAssociationTool

    # Position
    object.defineBlock (flagTestLOD('vertexPositionLevelOfDetails>=1', flags),
                        'Position',
                        TrackD3PDMaker.VertexPositionFillerTool)

    # Covariance Matrix
    CovarianceAssoc = SimpleAssociation \
                      (object,
                       TrackD3PDMaker.VertexCovarianceAssociationTool,
                       level = flagTestLOD('vertexPositionLevelOfDetails>=2', flags))


    CovarianceAssoc.defineBlock (
        flagTestLOD('vertexPositionLevelOfDetails>=2', flags),
        'Error',
        TrackD3PDMaker.CovarianceFillerTool,
        Error = deferFlag ('storeDiagonalCovarianceAsErrors', flags),
        DiagCovariance = deferFlag('not storeDiagonalCovarianceAsErrors',flags),
        OffDiagCovariance = False,
        IsPosition = True)

    CovarianceAssoc.defineBlock (flagTestLOD('vertexPositionLevelOfDetails>=3', flags),
                                 'Covariance',
                                 TrackD3PDMaker.CovarianceFillerTool,
                                 Error = False,
                                 DiagCovariance = False,
                                 OffDiagCovariance = True,
                                 IsPosition = True)

    # Type
    object.defineBlock (flagTestLOD('storeVertexType', flags),
                        'Type',
                        TrackD3PDMaker.VertexTypeFillerTool)
    
    # Fit Quality
    FitQualityAssoc = SimpleAssociation \
                      (object,
                       TrackD3PDMaker.VertexFitQualityAssociationTool,
                       level = flagTestLOD('storeVertexFitQuality', flags))
        
    FitQualityAssoc.defineBlock (flagTestLOD('storeVertexFitQuality', flags),
                                 'FitQuality',
                                 TrackD3PDMaker.TrackFitQualityFillerTool)

    # Kine
    object.defineBlock (flagTestLOD('storeVertexKinematics', flags),
                        'Kine',
                        TrackD3PDMaker.VertexKineFillerTool)

    # Purity
    if rec.doTruth():
        object.defineBlock(flagTestLOD('doTruth and storeVertexPurity', flags, _get_purity),
                           'Purity',
                           TrackD3PDMaker.VertexPurityFillerTool)

    # Track association
    TrackAssoc = ContainedVectorMultiAssociation\
                 (object,
                  TrackD3PDMaker.VertexTrackAssociationTool,
                  trackPrefix,
                  FillUnbiasedIP = deferFlag ('storeVertexTrackAssociation',
                                              flags),
                  level = flagTestLOD ('storeVertexTrackAssociation or storeVertexTrackIndexAssociation',
                                       flags))

    TrackAssoc.defineBlock(flagTestLOD('storeVertexTrackAssociation', flags),
                           'TrackAtVertex',
                           TrackD3PDMaker.VertexTrackAssociationFillerTool)

    TrackParticleAssoc = IndexAssociation (
        TrackAssoc,
        vxTrackAtVertexTrackAssociationTool,
        trackTarget,
        level = flagTestLOD ('storeVertexTrackIndexAssociation', flags))


def BuildVertexD3PDObject(_prefix='vx_',
                          _label='vx',
                          _sgkey=D3PDMakerFlags.VertexSGKey(),
                          _object_name='PrimaryVertexD3PDObject',
                          trackTarget='trk',
                          trackPrefix='trk_',
                          trackType='Rec::TrackParticleContainer'):

    object = make_SGDataVector_D3PDObject (
        'VxContainer',
        _sgkey,
        _prefix,
        _object_name,
        default_allowMissing = True,
        allow_args = ['storeVertexTrackAssociation',
                      'storeVertexTrackIndexAssociation',
                      'storeDiagonalCovarianceAsErrors',
                      'storeVertexType',
                      'storeVertexFitQuality',
                      'storeVertexKinematics',
                      'storeVertexPurity',
                      'vertexPositionLevelOfDetails',
                      'doTruth'])

    DefineVertexD3PDObject(object,
                           trackTarget,
                           trackPrefix,
                           trackType)

    return object


#
# Additional arguments that can be passed to this D3PDObject:
#   storeVertexAssociation
#   storeVertexTrackIndexAssociation
#
PrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='vx_',
                                                _label='vx',
                                                _sgkey='VxPrimaryCandidate',
                                                _object_name='PrimaryVertexD3PDObject',
                                                trackTarget='trk',
                                                trackPrefix='trk_',
                                                trackType='Rec::TrackParticleContainer')
