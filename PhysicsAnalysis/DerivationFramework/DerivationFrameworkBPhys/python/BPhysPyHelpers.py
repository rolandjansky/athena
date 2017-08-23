# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# BPhysPyHelpers.py
#
# Python helper classes for BPHY derivations.
#
# authors: W. Walkowiak <wolfgang.walkowiak@cern.ch>, 2016-05-19
# changed:
#
# This file contains a set of Python helper classes for the
# BPHY derivations.
#
# Available helper classes and methods:
# - BPyWrapper -- wrap configurable to ensure default contents of
#                 __slots__ dict are available as attributes
#                 Usage example: see BPHY8.py
#                 Note: Unfortunately, this doesn't quite work, since the
#                 wrapped class is no longer recognized as Configurable
# - BPhysEnsureAttributes(algtool)
#               -- ensure default contents of __slots__ dict a
#                  are available as attributes to the class
#====================================================================

#--------------------------------------------------------------------
class BPyWrapper(object):
    __slots__ = {'wclass' : object} 

    def __init__(self, wclass, *args, **kargs):
        object.__setattr__(self, 'wclass', wclass(*args, **kargs))
        # the important part: make __slot__ variables attributes
        for n,v in self.wclass.__slots__.items():
            if not hasattr(self.wclass, n):
                setattr(self.wclass, n, v)

    def __getattr__(self, attr):
        return self.wclass.__getattribute__(attr)
      
    def __setattr__(self, attr, value):
        setattr(self.wclass, attr, value)
  
    def __call__(self, *args, **kwargs):
        return self.wclass(*args, **kwargs)
#--------------------------------------------------------------------
#
# ensure default contents of __slots__ dict are available as attributes
#
def BPhysEnsureAttributes(algtool):

    for n,v in algtool.__slots__.items():
        if not hasattr(algtool, n):
            setattr(algtool, n, v)
    return algtool
#--------------------------------------------------------------------
