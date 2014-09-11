# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## hook for the PerfMonEvent py-module

def setup():
    """bring in the extension module"""
    import os
    __path__.append( os.path.join( __path__[0], os.environ[ 'CMTCONFIG' ] ) )
    import DataStore
    return
setup()
del setup
