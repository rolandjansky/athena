# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/ZDCTriggerBitsD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2010
# @brief ZDC trigger bits.
#


from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject      import make_Void_D3PDObject

ZDCTriggerBitsD3PDObject = \
           make_Void_D3PDObject (default_object_name = 'ZDCTriggerBitsD3PDObject', default_name = 'ZDCTriggerBits')
defineTriggerBits (ZDCTriggerBitsD3PDObject, 0, 'L1_ZDC')
