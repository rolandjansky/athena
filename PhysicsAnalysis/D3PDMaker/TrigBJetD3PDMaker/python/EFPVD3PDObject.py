# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation

EFPVD3PDObject = make_SGDataVector_D3PDObject ('VxContainer',
                                               'HLT_PrimVx',
                                               'trig_EF_pv_',
                                               'EFPrimaryVertexD3PDObject',
                                               default_allowMissing = True)

EFPVD3PDObject.defineBlock (0, 'Position',
                                TrackD3PDMaker.VertexPositionFillerTool)
EFPVD3PDObject.defineBlock (0, 'Type',
                                TrackD3PDMaker.VertexTypeFillerTool)

CovarianceAssoc = SimpleAssociation(EFPVD3PDObject,
                                   TrackD3PDMaker.VertexCovarianceAssociationTool)
CovarianceAssoc.defineBlock (0, 'Error',
                             TrackD3PDMaker.CovarianceFillerTool,
                             Error = True,
                             DiagCovariance = False,
                             OffDiagCovariance = False,
                             IsPosition = True)

