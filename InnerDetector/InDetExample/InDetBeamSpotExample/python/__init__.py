# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Scripts to run beam spot production jobs
"""
__author__ = 'Juerg Beringer'


def loadClass(name):
    """Dynamically load a Python class from the InDetBeamSpotExample package. Returns a class object."""
    # See note in __import__ doc on when package or module is returned
    m = __import__('InDetBeamSpotExample.'+name, fromlist = [name])
    return getattr(m,name)
