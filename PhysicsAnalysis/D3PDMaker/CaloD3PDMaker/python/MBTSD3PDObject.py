# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/MBTSD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2009
# @brief D3PD object for MBTS.
#

# We require TileID.
# If identifiers have not yet been configured, request a minimal
# setup (that doesn't use GeoModelSvc).
from AthenaCommon.AppMgr import ServiceMgr 
if not hasattr (ServiceMgr, 'DetDescrCnvSvc'):
    import DetDescrCnvSvc.DetStoreConfig
    ServiceMgr.DetDescrCnvSvc.DoInitNeighbours = False

import CaloD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

MBTSD3PDObject = \
           make_SGDataVector_D3PDObject ('TileContainer<TileCell>',
                                         D3PDMakerFlags.MBTSSGKey(),
                                         'mb_', 'MBTSD3PDObject')

MBTSD3PDObject.defineBlock (0, 'MBTS', CaloD3PDMaker.MBTSFillerTool)

