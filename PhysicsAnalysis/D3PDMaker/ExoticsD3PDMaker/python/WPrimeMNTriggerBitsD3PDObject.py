# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file    ExoticsD3PDMaker/python/WPrimeMNTriggerBitsD3PDObject.py
# @author  Nikos Tsirintanis <Nikolaos.Tsirintanis@cern.ch>
# @date    May, 2012
# @brief   Define trigger bit blocks for WPrime->munu
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
def makeWPrimeMNTriggerBitsD3PDObject (name,
                                   prefix,
                                   object_name,
                                   getter = None,
                                   sgkey = ''):
  
    return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                 Prefix = prefix
                                                 )

# Create the object type.
WPrimeMNTriggerBitsD3PDObject = \
  D3PDObject (makeWPrimeMNTriggerBitsD3PDObject,
              default_name = 'WPrimeMNTriggerBitsFiller')


######
# Define blocks.

if D3PDMakerFlags.DoTrigger():

    defineTriggerBits (WPrimeMNTriggerBitsD3PDObject, 0,
                       [ "EF_mu36_tight",
                         "EF_mu50_MSonly_barrel_tight"])

    

     
