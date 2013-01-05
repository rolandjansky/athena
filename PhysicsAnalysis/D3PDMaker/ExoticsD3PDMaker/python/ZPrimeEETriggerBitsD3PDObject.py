# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ZPrimeEETriggerBitsD3PDObject.py 498626 2012-05-01 14:52:33Z addy $
#
# @file ExoticsD3PDMaker/python/ZPrimeEETriggerBitsD3PDObject.py
# @author Michiru Kaneda<Michiru.Kaneda@cern.ch>
# @date Jan, 2011
# @brief Define trigger bit blocks for ZPrimeEE

import D3PDMakerCoreComps
import TriggerD3PDMaker
from TriggerD3PDMaker.defineTriggerBits   import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags

#
# The maker function.
# We don't get any input from SG (directly).
#
def makeZPrimeEETriggerBitsD3PDObject (name,
                                       prefix,
                                       object_name,
                                       getter = None,
                                       sgkey = ''):

    return D3PDMakerCoreComps.VoidObjFillerTool (name,
                                                 Prefix = prefix
                                                 )

# Create the object type.
ZPrimeEETriggerBitsD3PDObject = \
  D3PDObject (makeZPrimeEETriggerBitsD3PDObject,
              default_name = 'ZPrimeEETriggerBitsFiller')


######
# Define blocks.

if D3PDMakerFlags.DoTrigger():

    defineTriggerBits (ZPrimeEETriggerBitsD3PDObject, 0,
                         [
                         #EF
                         "EF_e22vh_loose",
                         "EF_e22vh_loose0",
                         "EF_e22vh_loose1",
                         "EF_e24vh_loose",
                         "EF_e24vh_loose0",
                         "EF_e24vh_loose1",
                         "EF_2e12Tvh_loose1",
                         "EF_e22vh_medium1_IDTrkNoCut",
                         "EF_e22vh_medium1",
                         "EF_e22vhi_medium1",
                         "EF_e24vh_medium1",
                         "EF_e24vhi_medium1",
                         "EF_e45_medium1",
                         "EF_e60_medium1",
                         "EF_e18_medium1_g25_loose",
                         "EF_g20_etcut",
                         "EF_g200_etcut",
                         "EF_2g40_loose",
                         "EF_g30_loose_g20_loose",
                         "EF_g35_loose_g30_loose",
                         "EF_g35_loose_g25_loose",
                         "EF_g40_loose_g25_loose",
                         "EF_g40_loose_g30_loose",
                         "EF_e18_loose1_g25_medium",
                         "EF_e18_loose1_g35_loose",
                         "EF_e18_loose1_g35_medium",
                         "EF_g20_loose",
                         "EF_g30_loose",
                         "EF_g40_loose",
                         "EF_g60_loose",
                         "EF_g80_loose",
                         "EF_g100_loose",
                         "EF_g120_loose",
                         "EF_g30_medium_g20_medium",
                         "EF_2g20vh_medium",
                         "EF_g20_medium",
                         "EF_g30_medium",                        
                         "EF_e18_loose1",
                         "EF_e18_medium1",
                         "EF_e18vh_medium1", 

                         #L2
                         "L2_e22vh_loose",
                         "L2_e22vh_loose0",
                         "L2_e22vh_loose1",
                         "L2_e24vh_loose",
                         "L2_e24vh_loose0",
                         "L2_e24vh_loose1",
                         "L22e12Tvh_loose1",
                         "L2_e22vh_medium1_IDTrkNoCut",
                         "L2_e22vh_medium1",
                         "L2e22vhi_medium1",
                         "L2_e24vh_medium1",
                         "L2_e24vhi_medium1",
                         "L2_e45_medium1",
                         "L2_e60_medium1",
                         "L2_e18_medium1_g25_loose",
                         "L2_g20_etcut",
                         "L2_g200_etcut",
                         "L2_2g40_loose",
                         "L2_g30_loose_g20_loose",
                         "L2_g35_loose_g30_loose",
                         "L2_g35_loose_g25_loose",
                         "L2_g40_loose_g25_loose",
                         "L2_g40_loose_g30_loose",
                         "L2_e18_loose1_g25_medium",
                         "L2_e18_loose1_g35_loose",
                         "L2_e18_loose1_g35_medium",
                         "L2_g20_loose",
                         "L2_2g20_loose",
                         "L2_g30_loose",
                         "L2_g40_loose",
                         "L2_g60_loose",
                         "L2_g80_loose",
                         "L2_g100_loose",
                         "L2_g120_loose",
                         "L2_g30_medium_g20_medium",
                         "L2_2g20vh_medium",
                         "L2_g20_medium",
                         "L2_g30_medium",
                         "L2_e18_loose1",
                         "L2_e18_medium1",
                         "L2_e18vh_medium1",
                         #L1
                         "L1_2EM10VH",
                         "L1_2EM12",
                         "L1_EM10VH",
                         "L1_EM12",
                         "L1_EM14VH",
                         "L1_EM16V",
                         "L1_EM16VH",
                         "L1_EM18VH",
                         "L1_EM30",
                         ])

