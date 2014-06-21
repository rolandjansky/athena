# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file  TauD3PDMaker/python/TauAnalysisTriggerBitsD3PDObject.py
# @date    Nov, 2009
# @brief   Define trigger bit blocks for tau analyses. Some of these
# triggers will have nothing to do with taus, but will be electron
# or missing-Et triggers needed for W or Z analyses
#


from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject


# Create the object type.
TauAnalysisTriggerBitsD3PDObject = \
     make_Void_D3PDObject( "", "TauAnalysisTriggerBitsD3PDObject",
                           default_name = 'TauAnalysisTriggerBitsFiller' )


######
# Define blocks.

# some bits for tau-related analyses
# for now these are the jet triggers since they are not in JetD3PDMaker
from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
defineTriggerBits (TauAnalysisTriggerBitsD3PDObject, 1, 'L1_J.*')
defineTriggerBits (TauAnalysisTriggerBitsD3PDObject, 1, 'L1_2J.*')
defineTriggerBits (TauAnalysisTriggerBitsD3PDObject, 1, 'L1_3J.*')
defineTriggerBits (TauAnalysisTriggerBitsD3PDObject, 1, 'L1_4J.*')

defineTriggerBits (TauAnalysisTriggerBitsD3PDObject, 1, 'EF_g.*')

defineTriggerBits (TauAnalysisTriggerBitsD3PDObject, 1, 'EF_2mu10.*')

defineTriggerBits (TauAnalysisTriggerBitsD3PDObject, 1, 'EF_xs.*')

                                      








