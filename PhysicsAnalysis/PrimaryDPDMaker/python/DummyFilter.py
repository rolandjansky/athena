# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        DummyFilter.py
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This class is a short algorithm to select all events. It is
##              needed together with the ApplyPrescaler to apply a flat
##              prescale for all events.
##=============================================================================

__doc__ = """This class is a short algorithm to select all events. It is
needed together with the ApplyPrescaler to apply a flat
prescale for all events.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode


class DummyFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select all events. It is
    needed together with the ApplyPrescaler to apply a flat
    prescale for all events.
    """

    def __init__ ( self, name = "DummyFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(DummyFilter, self).__init__(**kw)

        return


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( '************************************' )
        self.nProcessed = 0
        
        return StatusCode.Success


    def execute(self):
        self.nProcessed += 1
        self.msg.debug( '==> execute %s on %r. event...', self.name(), self.nProcessed )
        self.setFilterPassed(True)
        
        return StatusCode.Success


    def finalize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> finalizing %s...', self.name() )
        self.msg.info( '************************************' )

        return StatusCode.Success

