# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Block.py 485961 2012-03-01 14:29:38Z krasznaa $
#
# @file D3PDMakerCoreComps/python/Block.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Hold information about one D3PD block.
#


class Block:
    """Hold information about one D3PD block.

    Attributes:
      name:   Name of this block.
      lod:    Level of detail.  See below.
      func:   Function to create the block Configurable.
      kw: Arguments to pass to the creation function.

    The creation function will be called like this:

       b.func (name, **b.kw)

    where name is the name for the tool.

    However, if b.func is a class deriving from D3PDObject, then the creation
    function will instead be called like this:

       b.func (level, name, parent_prefix, **b.kw)

    LOD is used to control whether this block should be included.

    In the simple case, this is an integer, and the block is included
    if the block's level of detail is less than or equal to the
    requested level of detail (unless the block was explicitly
    included or excluded).

    In the general case, LOD may be a function.  This is called with
    two arguments, the requested level of detail and the block filler
    arguments.  The requested level of detail will be an integer;
    it will be 999 if the block was explicitly included, and -999
    if the block was explicitly excluded.  The block filler arguments
    is a dictionary of keyword arguments.  The LOD function should
    return a boolean value saying whether or not the block should
    be included.  It may also alter the dictionary of arguments
    (this overrides all other argument settings).
"""
    def __init__ (self, name, lod, func, kw):
        self.name = name
        self.lod = lod
        self.func = func
        self.kw = kw
        return


