# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: Chain.py
# @purpose: simple wrapper around the TrigSteering dict. class to ease
#           loading of that class from python
# based on Sebastien Binet's file ...

__all__ = [ 'Chain', ]

def _loadCintexDict():
    """Simple wrapper around the loading of dictionary class for the
       Chain class.
    """
    import PyCintex
    PyCintex.Cintex.Enable()

    ##
    klass = PyCintex.makeClass( "HLT::Chain" )
    return klass

Chain = _loadCintexDict()
del _loadCintexDict
