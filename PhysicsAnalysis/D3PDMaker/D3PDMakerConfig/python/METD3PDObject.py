# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import MissingETD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

METD3PDObject = make_SG_D3PDObject ('MissingET',
                                    D3PDMakerFlags.MissingETSGKey(),
                                    'met_', 'METD3PDObject')


METD3PDObject.defineBlock (0, 'MET',
                           MissingETD3PDMaker.MissingETFillerTool)
