# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: RawInfoSummaryForTagD3PDObject.py 351806 2011-03-15 10:05:36Z krasznaa $
#
# @file BackgroundD3PDMaker/python/RawInfoSummaryForTagD3PDObject.py
# @author Mark Tibbets <mark.james.tibbetts@cern.ch>
# @date Feb, 2011
# @brief D3PD object for RawInfoSummaryForTag.
#

import D3PDMakerCoreComps    
import BackgroundD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject

RawInfoSummaryForTagD3PDObject = \
    make_SG_D3PDObject ('RawInfoSummaryForTag',
                                  'RawInfoSummaryForTag',
                                  'rawtag_', 'RawInfoSummaryForTagD3PDObject')

RawInfoSummaryForTagD3PDObject.defineBlock (0, 'RawTagInDetSP',
                            BackgroundD3PDMaker.RawTagInDetSPFillerTool)
