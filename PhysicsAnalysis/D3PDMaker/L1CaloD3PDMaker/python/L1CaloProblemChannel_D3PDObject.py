# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file L1CaloD3PDMaker/python/L1CaloProblemChannel_D3PDObject.py
# @author John Morris <john.morris@cern.ch>
# @date Feb 2011
# @brief Fills L1Calo Problem Channels
#

import D3PDMakerCoreComps
import L1CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

from IOVDbSvc.CondDB import conddb
conddb.addFolder('TRIGGER','/TRIGGER/L1Calo/V1/Conditions/DisabledTowers')

# Maker
def makeL1CaloProblemChannel_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix,
                                                ObjectName = object_name)

# create the object type
L1CaloProblemChannel_D3PDObject = D3PDObject(makeL1CaloProblemChannel_D3PDObject,
                                  default_object_name = 'L1CaloProblemChannel_D3PDObject',
                                  default_name = 'L1CaloProblemChannelFiller')

# define blocks
L1CaloProblemChannel_D3PDObject.defineBlock(lod = 0,
                                   name = 'L1CaloProblemChannel',
                                   func = L1CaloD3PDMaker.L1CaloProblemChannelFillerTool,
                                   prefix = '')