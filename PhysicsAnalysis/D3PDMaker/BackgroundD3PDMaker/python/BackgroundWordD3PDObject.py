# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: BackgroundWordD3PDObject.py 351806 2011-03-15 10:05:36Z krasznaa $
#
# @file BackgroundD3PDMaker/python/BackgroundWordD3PDObject.py
# @author Mark Tibbets <mark.james.tibbetts@cern.ch>
# @date Mar, 2011
# @brief D3PD object for EventInfo Background Word.
#

import D3PDMakerCoreComps    
import BackgroundD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject

BackgroundWordD3PDObject = \
    make_SG_D3PDObject ('EventInfo',
                                  D3PDMakerFlags.EventInfoSGKey(),
                                  'bkgword_', 'BackgroundWordD3PDObject')

BackgroundWordD3PDObject.defineBlock (0, 'BackgroundWord',
                            BackgroundD3PDMaker.BackgroundWordFillerTool)
