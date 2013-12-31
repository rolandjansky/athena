# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file  QcdD3PDMaker/python/QcdTriggerBitsD3PDObject.py
# @author  Toshi Sumida <Toshi.Sumida@cern.ch>
# @date    Oct, 2012
# @brief   Define trigger bits for Qcd.
#

import D3PDMakerCoreComps
import TriggerD3PDMaker
from TriggerD3PDMaker.defineTriggerBits   import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags

#
# The maker function.
# We don't get any input from SG (directly).
#
def makeQcdTriggerBitsD3PDObject (name,
                                   prefix,
                                   object_name,
                                   getter = None,
                                   sgkey = ''):
    return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                 Prefix = prefix
                                                 )
# Create the object type.
QcdTriggerBitsD3PDObject = D3PDObject (makeQcdTriggerBitsD3PDObject,
                                       default_name = 'QcdTriggerBitsFiller')

######
# Define blocks.

if D3PDMakerFlags.DoTrigger():
    defineTriggerBits (QcdTriggerBitsD3PDObject, 0,
                       ['EF_.*']) # filter for "a4"ntiKt
