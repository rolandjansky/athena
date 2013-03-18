# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $ID$
#
# @file L1CaloD3PDMaker/python/L1CaloPPMdbMC_D3PDObject.py
# @author John Morris <john.morris@cern.ch>
# @brief Fills L1Calo PPM Database information
#

import D3PDMakerCoreComps
import L1CaloD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


# Maker
def makeL1CaloPPMdbMC_D3PDObject(name, prefix, object_name, getter = None, sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool(name, Prefix = prefix,
                                                ObjectName = object_name)

# create the object type
L1CaloPPMdbMC_D3PDObject = D3PDObject(makeL1CaloPPMdbMC_D3PDObject,
                                    default_object_name = 'L1CaloPPMdbMC_D3PDObject',
                                    default_name = 'L1CaloPPMdbMCFiller')

# define blocks

L1CaloPPMdbMC_D3PDObject.defineBlock(0,'L1CaloPPMdbMC',
                                   func = L1CaloD3PDMaker.L1CaloPPMdbMCFillerTool,
                                   prefix = '')


