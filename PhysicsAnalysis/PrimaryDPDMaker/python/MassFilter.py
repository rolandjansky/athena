# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        MassFilter
##
## Author:      Karsten Koeneke (CERN)
## Created:     July 2014
##
## Description: This is a short algorithm to select events that contain an
##              object that passes a minimum mass requirement.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain an
object that passes a minimum mass requirement.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

import math


class MassFilter( PyAthena.AthFilterAlgorithm ):
    """
    This is a short algorithm to select events that contain an
    object that passes a minimum mass requirement.
    The possible criteria are (with default values):

    the type of container by its collectionName:
    InputContainer = ''
    MinMassCut     = -10000.0*Units.GeV
    """



    def __init__ ( self, name = "MassFilter", **kw ):
        # initialize base class
        kw['name'] = name
        super(MassFilter, self).__init__(**kw)

        # Define the cuts
        self.inputCollectionName = kw.get('InputContainer', "")
        self.cutMassLower        = kw.get('MinMassCut', -10000.0*Units.GeV)
        self.storeGateSvc        = None


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  using input collection = %r', self.inputCollectionName )
        self.msg.info( '  MassLower              = %r', self.cutMassLower )
        self.msg.info( '************************************' )

        # Also, declare some counters and initialize them to zero
        self.nProcessed       = 0
        self.nEventPassMass   = 0

        # Get the StoreGate service
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if self.sg is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure

        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )

        # Then... here we go!
        acceptEvent  = False
        # Get the electron collections from StoreGate
        inputCollection = None
        try:
            inputCollection  = self.sg[self.inputCollectionName]
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.inputCollectionName )
            self.setFilterPassed(False)
            return StatusCode.Success

        # Do the combinations
        for part in inputCollection :
            if part.m() > self.cutMassLower :
                acceptEvent = True
                break
            pass # End: Loop over particles


        ## Check if the event is accepted
        if acceptEvent :
            self.msg.debug( '%s event passed' % self.name() )
            self.nEventPassMass += 1
            self.setFilterPassed(True)
            pass
        else:
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
            pass
        return StatusCode.Success



    def finalize(self):
        self.msg.debug( 'Saw %s events and accepted %s events' % (self.nProcessed, self.nEventPassMass) )

        return StatusCode.Success
