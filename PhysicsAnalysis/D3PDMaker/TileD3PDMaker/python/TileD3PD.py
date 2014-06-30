# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: iso-8859-1 -*-
#
# File: TileD3PD.py
# Author: Marco van Woerden <mvanwoer@cern.ch>
# Date: September 2012
#

import TileD3PDMaker.TileD3PDObject
from TileD3PDMaker.TileD3PDObject     import *
from TileD3PDMaker                                import *
from D3PDMakerCoreComps.resolveSGKey              import testSGKey

def mu_D3PD (alg = None,
              file = 'tile.root',
              tuplename = 'calo'):

    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )
        alg += TileD3PDMaker.TileD3PDObject.TileCellD3PDObject(10)
        alg += TileD3PDMaker.TileD3PDObject.TileMBTSD3PDObject(10)
        alg += TileD3PDMaker.TileD3PDObject.TileStacoMuonD3PDObject(10)
        alg += TileD3PDMaker.TileD3PDObject.TileEventD3PDObject(10)

        return alg

def Ep_D3PD (alg = None,
              file = 'tile.root',
              tuplename = 'calo'):

    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )
        alg += TileD3PDMaker.TileD3PDObject.TileTrackD3PDObject(10)
        alg += TileD3PDMaker.TileD3PDObject.TileCellD3PDObject(10)
        alg += TileD3PDMaker.TileD3PDObject.TileEventD3PDObject(10)
        #alg += TileD3PDMaker.TileD3PDObject.TileMuonD3PDObject(10)
        alg += TileD3PDMaker.TileD3PDObject.TileClusterD3PDObject(10)

        return alg
