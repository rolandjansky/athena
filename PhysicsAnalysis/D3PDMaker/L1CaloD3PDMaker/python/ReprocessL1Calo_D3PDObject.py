# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file L1CaloD3PDMaker/python/ReprocessL1Calo_D3PDObject.py
# @author John Morris <john.morris@cern.ch>
# @date May 2011
# @brief Reprocess L1Calo from ADC to RoI
#

import D3PDMakerCoreComps
import L1CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject



# Maker
def makeReprocessL1Calo_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix,
                                                ObjectName = object_name)

# create the object type
ReprocessL1Calo_D3PDObject = D3PDObject(makeReprocessL1Calo_D3PDObject,
                                  default_object_name = 'ReprocessL1Calo_D3PDObject',
                                  default_name = 'ReprocessL1CaloFiller')

# define blocks
ReprocessL1Calo_D3PDObject.defineBlock(lod = 0,
                                 name = 'ReprocessL1Calo',
                                 func = L1CaloD3PDMaker.ReprocessL1CaloFillerTool,
                                 ReprocessName = 'normal',
                                 prefix = '')
