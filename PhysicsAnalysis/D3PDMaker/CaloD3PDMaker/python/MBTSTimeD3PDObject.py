# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/MBTSTimeD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief MBTS time info object.
#

import CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject

MBTSTimeD3PDObject = \
           make_Void_D3PDObject ('mbtime_', 'MBTSTimeD3PDObject')
MBTSTimeD3PDObject.defineBlock (0, 'MBTSTime',
                                CaloD3PDMaker.MBTSTimeFillerTool)
