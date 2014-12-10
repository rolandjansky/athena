# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: reset_props.py 618070 2014-09-22 19:05:34Z ssnyder $
#
# @file D3PDMakerCoreComps/python/reset_props.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Reset properties of a configurable to default.
#


def reset_props (c):
    """Reset properties of a configurable to default.

    Useful when building private tools.  I.e.,

      tool1 = Tool(name1)
      tool.Foo = 1
      alg1.Tools += [tool1]

      tool2 = Tool(name2)
      alg2.Tools += [tool2]

    tool2 shouldn't have the same setting of Foo, even if name1 and name2
    are the same.
"""
    
    for p in c.getValuedProperties().keys():
        if p in ['EvtStore', 'DetStore', 'UserStore']: continue
        c.__delattr__ (p)
    c.removeAll()
    return
