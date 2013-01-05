# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ZPrimeMMTriggerBitsD3PDObject.py 480243 2012-02-01 16:37:27Z wanghill $
#
# @file    ExoticsD3PDMaker/python/ZPrimeMMTriggerBitsD3PDObject.py
# @author  Hulin Wang <Hulin.Wang@cern.ch>
# @date    Jan, 2012
# @brief   Define trigger bit blocks for ZPrimeMM
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
def makeZPrimeMMTriggerBitsD3PDObject (name,
                                   prefix,
                                   object_name,
                                   getter = None,
                                   sgkey = ''):
  
    return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                 Prefix = prefix
                                                 )

# Create the object type.
ZPrimeMMTriggerBitsD3PDObject = \
  D3PDObject (makeZPrimeMMTriggerBitsD3PDObject,
              default_name = 'ZPrimeMMTriggerBitsFiller')


######
# Define blocks.

if D3PDMakerFlags.DoTrigger():

    defineTriggerBits (ZPrimeMMTriggerBitsD3PDObject, 0,
                       ['EF_.*'])

    ##bjet bits
    defineTriggerBits (ZPrimeMMTriggerBitsD3PDObject, 0,
                       [ "L1_J10",
                         "L1_JE100",
                         "L1_JE140",
                         "L1_2J10_J50",
                         "L1_3J15",
                         "L1_3J20",
                         "L1_4J10",
                         "L1_4J15",
                         # Level 2
                         "L2_2b10_medium_L1JE100" ,
                         "L2_2b10_medium_L1JE140" ,
                         "L2_2b15_medium_3L1J15" ,
                         "L2_2b20_medium_3L1J20" ,
                         "L2_2b10_medium_4L1J10" ,
                         "L2_2b10_medium_L1_2J10J50",
                         "L2_3b10_loose_4L1J10",
                         "L2_3b15_loose_4L1J15",
                         # Event Filter 
                         'EF_b*',
                         'EF_2b*',
                         'EF_3b*'
                         ])

     
