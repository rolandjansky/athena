# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/MBTSTriggerBitsD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief MBTS trigger bits.
#

import TriggerD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject

MBTSTriggerBitsD3PDObject = \
           make_Void_D3PDObject (default_object_name = 'MBTSTriggerBitsD3PDObject', default_name = 'MBTSTriggerBits')
MBTSTriggerBitsD3PDObject.defineBlock (0, 'MBTSTriggerBits',
                                       TriggerD3PDMaker.TriggerBitFillerTool,
                                       Triggers = ['L1_MBTS_.*'])

