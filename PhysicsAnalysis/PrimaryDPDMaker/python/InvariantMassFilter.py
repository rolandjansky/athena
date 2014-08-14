# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        InvariantMassFilter
##
## Author:      Karsten Koeneke (CERN)
## Created:     August 2010
##
## Description: This is a short algorithm to select events that contain a 
##              combination of two objects with a certain invariant mass.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain a
combination of two objects with a certain invariant mass.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

import math


class InvariantMassFilter( PyAthena.AthFilterAlgorithm ):
    """
    This is a short algorithm to select events that contain a
    combination of two objects with a certain invariant mass.
    The possible criteria are (with default values):

    the type of container by its collectionName:
    inputCollection1Type    = 'ElectronContainer'
    inputCollection2Type    = 'ElectronContainer'
    inputCollection1Name    = 'ElectronAODCollection'
    inputCollection2Name    = 'ElectronAODCollection'
    
    di-electron mass lower bound:
    cutDiElMassLower        = 60.0*Units.GeV

    di-electron mass upper bound:
    cutDiElMassUpper        = 120.0*Units.GeV

    pass all events if true:
    passAll                 = False
    """



    def __init__ ( self, name = "InvariantMassFilter", **kw ):
        # initialize base class
        kw['name'] = name
        super(InvariantMassFilter, self).__init__(**kw)

        # Define the cuts
        self.inputCollection1Type   = kw.get('inputCollection1Type', "CaloClusterContainer")
        self.inputCollection1Name   = kw.get('inputCollection1Name', "egClusterCollection")
        self.inputCollection2Type   = kw.get('inputCollection2Type', "ElectronContainer")
        self.inputCollection2Name   = kw.get('inputCollection2Name', "ElectronAODCollection")
        self.cutMassLower           = kw.get('cutMassLower', -10000.0*Units.GeV)
        self.cutMassUpper           = kw.get('cutMassUpper', 10000.0*Units.TeV)
        self.passAll                = kw.get('passAll', False)
        self.storeGateSvc     = None


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  using input collection 1   = %r %r' % (self.inputCollection1Type, self.inputCollection1Name) )
        self.msg.info( '  using input collection 2   = %r %r' % (self.inputCollection2Type, self.inputCollection2Name) )
        self.msg.info( '  MassLower                  = %r', self.cutMassLower )
        self.msg.info( '  MassUpper                  = %r', self.cutMassUpper )
        self.msg.info( '  passAll                    = %r', self.passAll )
        self.msg.info( '************************************' )

        # Also, declare some counters and initialize them to zero
        self.nProcessed       = 0
        self.nEventPassMass   = 0

        # Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        # Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        from ROOT import TLorentzVector
        nPassMass   = 0

        
        # If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        # Then... here we go!
        acceptEvent  = False
        # Get the electron collections from StoreGate
        inputCollection1 = None
        try:
            inputCollection1  = self.storeGateSvc.retrieve( self.inputCollection1Type, self.inputCollection1Name )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.inputCollection1Name )           
            self.setFilterPassed(False)
            return StatusCode.Success

        inputCollection2 = None
        try:
            inputCollection2 = self.storeGateSvc.retrieve( self.inputCollection2Type, self.inputCollection2Name )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.inputCollection2Name )           
            self.setFilterPassed(False)
            return StatusCode.Success

        # Do the combinations
        for object1 in inputCollection1 :
            # Stop the loop when a good candidate was found
            if not acceptEvent :
                for object2 in inputCollection2 :
                    # Stop the loop when a good candidate was found
                    if not acceptEvent :
                        invmass = ( object1.hlv() + object2.hlv() ).m()
                        # Select events inside the invariant di-electron mass window
                        if invmass > self.cutMassLower and invmass < self.cutMassUpper :
                            nPassMass += 1
                            acceptEvent = True
                            pass  # End: Check Mass 
                        pass # End: break loop if event was already accepted
                    pass # End: Loop over object2
                pass # End: break loop if event was already accepted
            pass # End: Loop over object1
        

        ## Translate the electron pass counters into event pass counters
        if nPassMass > 0 :   self.nEventPassMass   += 1

        ## Check if the event is accepted
        if acceptEvent :
            self.msg.debug( '%s event passed' % self.name() )
            self.setFilterPassed(True)
            pass
        else:
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
            pass
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiPassMass      = 0.0
        effiErrPassMass      = 0.0
        try :
            # Efficiencies
            effiPassMass      = 100.0*self.nEventPassMass / self.nProcessed

            # Error on efficiencies
            effiErrPassMass      = 100.0 * math.sqrt(self.nEventPassMass) / self.nProcessed
            pass
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Events after di-em invariant mass cut:       %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassMass,      effiPassMass,      effiErrPassMass ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

