# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file  TauD3PDMaker/python/TauTriggerBitsD3PDObject.py
# @date    Nov, 2009
# @brief   Define trigger bit blocks for Tau.
#


import TriggerD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject


# Create the object type.
TauTriggerBitsD3PDObject = \
       make_Void_D3PDObject(  "", "TauTriggerBitsD3PDObject",
                              default_name = 'TauTriggerBitsFiller' )

######
# Define blocks.


TauTriggerBitsD3PDObject.defineBlock \
                                     (0, 'TauBits0',
                                      TriggerD3PDMaker.TriggerBitFillerTool ,
                                      Triggers=[
                                          'EF_tau12_loose',
                                          'EF_tau16_loose',
                                          'EF_tau16i_loose',
                                          'EF_tau16i_loose_cosmic',
                                          'EF_tau16_loose_cosmic',
                                          'EF_tau20_loose',
                                          'EF_tau20i_loose',
                                          'EF_tau29_loose',
                                          'EF_tau29i_loose',
                                          'EF_tau38_loose',
                                          'EF_tau50_loose',
                                          'EF_tau84_loose',
                                          'EF_tau125_loose',
                                          'EF_tau12_loose_PT',
                                          'EF_tau16_loose_PT',
                                          'EF_tau50_loose_PT',
                                          'EF_tau16i_medium',
                                          'EF_tau20i_medium',
                                          'EF_tau29i_medium',
                                          'EF_tau38_medium', 
                                          'EF_tau50_medium',
                                          'EF_tau84_medium',
                                          'EF_tau125_medium',
                                          'EF_tau16i_tight',
                                          'EF_tau20i_tight',
                                          'EF_tau29i_tight',
                                          'EF_tau38_tight',
                                          'EF_tau50_tight',
                                          'EF_tau84_tight',
                                          'EF_tau125_tight',
                                          ]
                                    )


TauTriggerBitsD3PDObject.defineBlock \
                                     (1, 'TauBits1',
                                      TriggerD3PDMaker.TriggerBitFillerTool ,
                                      Triggers=[
                                          'L1_TAU6',
                                          'L1_TAU9',
                                          'L1_TAU9I',
                                          'L1_TAU16',
                                          'L1_TAU16I',
                                          'L1_TAU25',
                                          'L1_TAU40',
                                          'L1_2TAU6I',
                                          'L1_2TAU5',
                                          'L1_2TAU6',
                                          'L2_tau12_loose',
                                          'L2_tau16_loose',
                                          'L2_tau16i_loose',
                                          'L2_tau16i_loose_cosmic',
                                          'L2_tau16_loose_cosmic',
                                          'L2_tau20_loose',
                                          'L2_tau20i_loose',
                                          'L2_tau29_loose', 
                                          'L2_tau29i_loose', 
                                          'L2_tau38_loose', 
                                          'L2_tau50_loose',
                                          'L2_tau84_loose',
                                          'L2_tau125_loose',
                                          'L2_tau12_loose_PT',
                                          'L2_tau16_loose_PT',
                                          'L2_tau50_loose_PT',
                                          'L2_tau16i_medium',
                                          'L2_tau20i_medium',
                                          'L2_tau29i_medium',
                                          'L2_tau38_medium', 
                                          'L2_tau50_medium',
                                          'L2_tau84_medium',
                                          'L2_tau125_medium',
                                          'L2_tau16i_tight',
                                          'L2_tau20i_tight',
                                          'L2_tau29i_tight',
                                          'L2_tau38_tight', 
                                          'L2_tau50_tight', 
                                          'L2_tau84_tight',
                                          'L2_tau125_tight',
                                          ]
                                      )
