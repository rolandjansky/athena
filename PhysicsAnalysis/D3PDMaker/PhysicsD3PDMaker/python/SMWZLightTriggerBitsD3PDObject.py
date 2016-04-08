# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: SMWZLightTriggerBitsD3PDObject.py 467769 2011-11-11 22:36:30Z boonekam $
#
# @file    PhysicsD3PDMaker/python/SMWZLightTriggerBitsD3PDObject.py
# @author  Haifeng Li <Haifeng.Li@cern.ch>
# @date    May, 2010
# @brief   Define trigger bit blocks for SMWZ
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
def makeSMWZLightTriggerBitsD3PDObject (name,
                                   prefix,
                                   object_name,
                                   getter = None,
                                   sgkey = ''):
  
    return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                 Prefix = prefix
                                                 )

# Create the object type.
SMWZLightTriggerBitsD3PDObject = \
  D3PDObject (makeSMWZLightTriggerBitsD3PDObject,
              default_name = 'SMWZLightTriggerBitsFiller')


######
# Define blocks.

if D3PDMakerFlags.DoTrigger():
    
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_e*'])
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_2e*'])
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_g*'])
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_2g*'])
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_mu*'])
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_2mu*'])
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_xe*'])
    defineTriggerBits (SMWZLightTriggerBitsD3PDObject, 0, ['EF_xs*'])
        
