# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagEventInfoD3PDObject.py
## @brief event info D3PD object configured for the btagging D3PD
## @author Georges Aad
## @date Nov, 2010
##

import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject

def getJetTagEventInfoD3PDObject(level=1, **kw):

    ### for now return the EventInfoD3PDObject as it is. 
    ### keep this just in case we want to change the default.

    return EventInfoD3PDObject(level=level, **kw)






