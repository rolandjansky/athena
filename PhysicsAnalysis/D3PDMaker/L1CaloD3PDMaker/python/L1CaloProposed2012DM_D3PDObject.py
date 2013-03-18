# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file L1CaloD3PDMaker/python/Proposed2012DM_D3PDObject.py
# @author John Morris <john.morris@cern.ch>
# @date Nov 2011
# @brief Impliments reprocessed L1Calo RoIs for the preposed 2012 Dead Material corrections
#

import D3PDMakerCoreComps
import L1CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


# Maker
def makeL1CaloProposed2012DM_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix,
                                                ObjectName = object_name)

# create the object type
L1CaloProposed2012DM_D3PDObject = D3PDObject(makeL1CaloProposed2012DM_D3PDObject,
                                  default_object_name = 'L1CaloProposed2012DM_D3PDObject',
                                  default_name = 'ReprocessL1CaloFiller')

# define blocks
L1CaloProposed2012DM_D3PDObject.defineBlock(lod = 0,
                                 name = 'ReprocessL1Calo_DM',
                                 func = L1CaloD3PDMaker.ReprocessL1CaloFillerTool,
                                 ReprocessName = 'dm',
                                 DoPPM = False,
                                 DoCPM = False,
                                 DoJEM = False,
                                 DoCMM = False,
                                 DoROI = True,
                                 prefix = 'trig_L1_DM_')

L1CaloProposed2012DM_D3PDObject.defineBlock(lod = 1,
                                 name = 'ReprocessL1Calo_DMScaled',
                                 func = L1CaloD3PDMaker.ReprocessL1CaloFillerTool,
                                 ReprocessName = 'dmScaled',
                                 DoPPM = False,
                                 DoCPM = False,
                                 DoJEM = False,
                                 DoCMM = False,
                                 DoROI = True,
                                 prefix = 'trig_L1_DMScaled_')                                 
