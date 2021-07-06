# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# @file MuonD3PDMaker/python/MuonSpShowerD3PDObject.py
# @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
# @date Jan., 2011
# @brief MuonSpShower filler tool
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from MuonD3PDMaker.MuonD3PDMakerConf import D3PD__MuonSpShowerFillerTool as MuonSpShowerFillerTool

MuonSpShowerD3PDObject = \
           make_SGDataVector_D3PDObject ('Rec::MuonSpShowerContainer',
                                         'MuonSpShowers',
                                         'musp_', 'MuonSpShowerD3PDObject')

MuonSpShowerD3PDObject.defineBlock(0, 'Position',
                           MuonSpShowerFillerTool,
                           WritePosition  = True,
                           WriteHits = False,
                           WriteSegments = False)

MuonSpShowerD3PDObject.defineBlock(1, 'Hits',
                           MuonSpShowerFillerTool,
                           WritePosition  = False,
                           WriteHits = True,
                           WriteSegments = False)

MuonSpShowerD3PDObject.defineBlock(1, 'Segments',
                           MuonSpShowerFillerTool,
                           WritePosition  = False,
                           WriteHits = False,
                           WriteSegments = True)


