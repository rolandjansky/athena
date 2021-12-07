# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.flagTestLOD import flagTestLOD, deferFlag
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
from InDetRecExample.TrackingCommon import getInDetTrackToVertexTool


def DefinexAODVertexD3PDObject(object,
                               trackTarget='trk',
                               trackPrefix='trk_',
                               flags=TrackD3PDFlags):

    # Position
    object.defineBlock (flagTestLOD('vertexPositionLevelOfDetails>=1', flags),
                        'Position',
                        D3PDMakerCoreComps.AuxDataFillerTool,
                        Vars = ['x', 'y', 'z'])

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
                        D3PDMakerCoreComps.AuxDataFillerTool,
                        Vars = ['type = vertexType'])
    
    # Fit Quality
    object.defineBlock (flagTestLOD('storeVertexFitQuality', flags),
                        'FitQuality',
                        D3PDMakerCoreComps.AuxDataFillerTool,
                        Vars = ['chi2 = chiSquared', 'ndof = numberDoF'])

    # Kine
    object.defineBlock (flagTestLOD('storeVertexKinematics', flags),
                        'Kine',
                        TrackD3PDMaker.VertexKineFillerTool)

    # Track Association
    TrackAssoc = ContainedVectorMultiAssociation \
                 (object,
                  TrackD3PDMaker.VertexTrackParticleAssociationTool,
                  trackPrefix,
                  level = flagTestLOD ('storeVertexTrackAssociation or storeVertexTrackIndexAssociation',
                                       flags))

    TrackAssoc.defineBlock (
        flagTestLOD ('storeVertexTrackAssociation or storeVertexTrackIndexAssociation', flags),
        'TrkFitQuality',
        D3PDMakerCoreComps.AuxDataFillerTool,
        Vars = ['chi2 = chiSquared'])

    PerigeeAssoc = SimpleAssociation \
                   (TrackAssoc,
                    TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
                    TrackToVertexTool = getInDetTrackToVertexTool())
    PerigeeAssoc.defineBlock (flagTestLOD ('storeVertexTrackAssociation or storeVertexTrackIndexAssociation', flags),
                              'Trk',
                              TrackD3PDMaker.PerigeeFillerTool)

    TrackAssoc.defineBlock (
        flagTestLOD ('storeVertexTrackIndexAssociation', flags),
        'TrackAssocIndex',
        D3PDMakerCoreComps.IndexFillerTool,
        Target = trackTarget)


def BuildxAODVertexD3PDObject(_prefix='vx_',
                              _label='vx',
                              _sgkey=D3PDMakerFlags.VertexSGKey(),
                              _object_name='PrimaryVertexD3PDObject',
                              trackTarget='trk',
                              trackPrefix='trk_'):

    object = make_SGDataVector_D3PDObject (
        'DataVector<xAOD::Vertex_v1>',
        _sgkey,
        _prefix,
        _object_name,
        #default_allowMissing = True,
        allow_args = ['storeVertexTrackAssociation',
                      'storeVertexTrackIndexAssociation',
                      'storeDiagonalCovarianceAsErrors',
                      'storeVertexType',
                      'storeVertexFitQuality',
                      'storeVertexKinematics',
                      'storeVertexPurity',
                      'vertexPositionLevelOfDetails',
                      'doTruth'])

    DefinexAODVertexD3PDObject(object,
                               trackTarget,
                               trackPrefix)

    return object


#
# Additional arguments that can be passed to this D3PDObject:
#   storeVertexAssociation
#   storeVertexTrackIndexAssociation
#
PrimaryxAODVertexD3PDObject = BuildxAODVertexD3PDObject(
    _prefix='vx_',
    _label='vx',
    _sgkey='VxPrimaryCandidate',
    _object_name='PrimaryVertexD3PDObject',
    trackTarget='trk',
    trackPrefix='trk_')
