# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file  TriggerD3PDMaker/python/addTriggerBits.py
# @author scott snyder <snyder@bnl.gov>
# @date Apr, 2010
# @brief Helper to request trigger bits in the D3PD, avoiding duplicates.
#
# TriggerBitFillerTool is used to record trigger decisions in the D3PD.
# It takes a list of patterns, and adds all matching trigger bits
# as D3PD variables.
#
# However, especially when patterns are used, there can be an issue with
# the same trigger bit being requested from more than one place.
# This will cause the D3PD framework to complain about a duplicate variable.
#
# To address this, use defineTriggerBits instead.  This manages
# a single TriggerBitsFillerTool; duplicates will be removed within
# the tool so that's no longer an issue.
#
# Use it like this:
#
#  defineTriggerBits (d3pdObj, 0, 'L1_EM.*')
#


import TriggerD3PDMaker
from D3PDMakerCoreComps.D3PDObject  import make_Void_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags


triggerBitsD3PDObject = \
  make_Void_D3PDObject ( "", "triggerBitsD3PDObject",
                         default_name = 'TriggerBitsFiller' )


triggerBitsD3PDObject.defineBlock \
  (0, 'TriggerBits',
   TriggerD3PDMaker.TriggerBitFillerTool ,
   AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
   Triggers=[ ])


def defineTriggerBits (d3pdobj, level, pattern):
    """Define trigger bits to be added to the D3PD whenever D3PDObj is added.

LEVEL is the level-of-detail for the bits.
PATTERN is a regular expression giving the pattern of the desired bits.
It may also be a list of patterns.

This function sets up a hook that runs when D3PDOBJ is instantiated.
The hook will first see if there is a global TriggerBitsFiller
tool already in the tree; if not, it will add one.  Then it will
add the requested bit name pattern to the tool's list.

If the tree's algorithm contains an attribute `trigDecisionTree',
then that should be the algorithm for another tree, which is used instead.
This allows one to segregate the trigger decision flags in a separate tree.
"""
    level_in = level
    def fn (c, level, **kw):
        # Should we include this pattern?
        if level < level_in: return

        # Find the algorithm for c.
        parentname = c.getParent()
        alg = c.allConfigurables[parentname]

        # If the algorithm defines a separate tree for trigger decisions,
        # use that instead.
        if hasattr (alg, 'trigDecisionTree'):
            alg = alg.trigDecisionTree
            pass

        # See if we have the filler tool.
        # Make a new one if needed.
        filler = getattr (alg, 'TriggerBitsFiller', None)
        if not filler:
            alg += triggerBitsD3PDObject (9999)
            filler = getattr (alg, 'TriggerBitsFiller')
            filler.TriggerBitsFiller_TriggerBits.Triggers = []
            pass

        pat = pattern
        if not isinstance(pat, list):
            pat = [pat]
            pass
        filler.TriggerBitsFiller_TriggerBits.Triggers += pat

        return
    d3pdobj.defineHook (fn)
    return
