# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file    ExoticsD3PDMaker/python/WPrimeENTriggerBitsD3PDObject.py
# @author  Nikos Tsirintanis <Nikolaos.Tsirintanis@cern.ch>
# @date    May, 2012
# @brief   Define trigger bit blocks for WPrime->enu
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
def makeWPrimeENTriggerBitsD3PDObject (name,
                                   prefix,
                                   object_name,
                                   getter = None,
                                   sgkey = ''):
  
    return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                 Prefix = prefix
                                                 )

# Create the object type.
WPrimeENTriggerBitsD3PDObject = \
  D3PDObject (makeWPrimeENTriggerBitsD3PDObject,
              default_name = 'WPrimeENTriggerBitsFiller')


######
# Define blocks.

if D3PDMakerFlags.DoTrigger():

    defineTriggerBits (WPrimeENTriggerBitsD3PDObject, 0,
                       [ "EF_2g20_loose",
					   "EF_g120_loose",
					   "EF_e20_loose", 
					   "EF_e20_medium",
					   "EF_e22_medium", 
					   "EF_g80_loose", 
					   "EF_e13_etcut_xs60_noMu", 
					   "EF_e13_etcut_xs60_noMu_dphi2j10xe07",
					   "EF_mu36_tight",
					   "EF_mu40_tight",
					   "EF_mu50_tight",
					   "EF_e24vhi_medium1",
					   "EF_e60_medium1",
					   "EF_e45_medium1"])

    

     
