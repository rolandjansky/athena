# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file L1CaloD3PDMaker/python/L1CaloNoFCALRoI_D3PDObject.py
# @author John Morris <john.morris@cern.ch>
# @date October 2011
# @brief L1Calo RoIs with no FCAL
#

import D3PDMakerCoreComps
import L1CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject

# Maker
def makeL1CaloNoFCALRoI_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix,
                                                ObjectName = object_name)

# create the object type
L1CaloNoFCALRoI_D3PDObject = D3PDObject(makeL1CaloNoFCALRoI_D3PDObject,
                                         default_object_name = 'L1CaloNoFCALRoI_D3PDObject',
                                         default_name = 'L1CaloNoFCALRoIsFiller')

# define blocks
L1CaloNoFCALRoI_D3PDObject.defineBlock(lod = 0,
                                       name = 'L1CaloNoFCALRoI',
                                       func = L1CaloD3PDMaker.L1CaloNoFCALRoIFillerTool)
