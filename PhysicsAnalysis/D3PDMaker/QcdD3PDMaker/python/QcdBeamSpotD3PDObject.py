# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Beam spot position and spread
import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import QcdD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker import TrackD3PDMakerConf

## BeamSpotD3PDObject = make_SG_D3PDObject ('VxContainer','MinBiasD3PD::BeamSpotRecVertex','beamSpot_')

QcdBeamSpotD3PDObject = make_SGDataVector_D3PDObject ('VxContainer',
                                                      'QcdD3PD::BeamSpotRecVertex',
                                                      'beamSpot_',
                                                      'QcdBeamSpotD3PDObject')

# Position
QcdBeamSpotD3PDObject.defineBlock (0, 'Position',
                                TrackD3PDMaker.VertexPositionFillerTool)

# Spread
QcdCovarianceAssoc = SimpleAssociation \
                  (QcdBeamSpotD3PDObject,
                   TrackD3PDMaker.VertexCovarianceAssociationTool)

QcdCovarianceAssoc.defineBlock (1, 'Error',
                             TrackD3PDMaker.CovarianceFillerTool,
                             Error = True,
                             DiagCovariance = True,
                             IsPosition = True)
