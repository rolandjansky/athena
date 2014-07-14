# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

JetD3PDObject = make_SGDataVector_D3PDObject ('JetCollection',
                                              D3PDMakerFlags.JetSGKey(),
                                              'jet_')

JetD3PDObject.defineBlock (0, 'Kinematics',
                           EventCommonD3PDMaker.FourMomFillerTool,
                           WriteE  = True)
