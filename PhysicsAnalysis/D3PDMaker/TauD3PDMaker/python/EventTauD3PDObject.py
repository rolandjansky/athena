# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file TauD3PDMaker/python/EventTauD3PDMaker.py
# @author Michel Trottier-McDonald <mtm@cern.ch>
# @date December 2010
# @brief Fills event-based information relevant to tau ID
#

import TauD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject


# Create the object type
EventTauD3PDObject = \
     make_Void_D3PDObject( "", "EventTauD3PDObject",
                           default_name = 'EventTauFiller' )


#define blocks
EventTauD3PDObject.defineBlock(lod = 1,
                               name = 'EventTau',
                               func = TauD3PDMaker.EventCalcVarsFillerTool,
                               prefix = 'evt_calcVars_')
