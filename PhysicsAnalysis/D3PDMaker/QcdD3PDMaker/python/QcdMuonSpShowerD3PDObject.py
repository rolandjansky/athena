# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file QcdD3PDMaker/python/QcdMuonSpShowerD3PDObject.py
# @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
# @date Jan., 2011
# @brief MuonSpShower filler tool
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from QcdD3PDMaker.QcdD3PDMakerConf import QcdD3PD__MuonSpShowerFillerTool as MuonSpShowerFillerTool

MuonSpShowerD3PDObject = make_SGDataVector_D3PDObject( "Rec::MuonSpShowerContainer",
                                                       "MuonSpShowers",
                                                       "musp_",
                                                       "QcdMuonSpShowerD3PDObject" )

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


