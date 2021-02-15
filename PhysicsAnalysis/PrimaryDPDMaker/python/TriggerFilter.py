# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Trigger Skimmer
##
## Author:      Eric Feng (Chicago)
## Created:     August 2008
##
## Based on:    Photon Skimmer by Karsten Koeneke (DESY)
##
## Description: This is a short algorithm to select events that pass a certain
##              set of triggers.
##=============================================================================

__doc__ = """This is a short algorithm to select events that pass at least
one out of a certain set of triggers.
"""
__version__ = "0.0.5"
__author__  = "Eric Feng <Eric.Feng@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class TriggerFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that pass at
    least one out of a certain set of triggers.  The possible
    criteria are (with default values):
    trigger          = ''
    passAll          = False
    """


    def __init__ ( self, name = "TriggerFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(TriggerFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.trigger      = kw.get( 'trigger', '' )
        self.passAll      = kw.get( 'passAll', False )

        ## TrigDecisionTool handle
        self.tdt          = None


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  trigger            = %r', self.trigger )
        self.msg.info( '  passAll            = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()  # noqa: F841

        ## Get the TrigDecisionTool tool
        self.tdt = PyAthena.py_tool('Trig::TrigDecisionTool/TrigDecisionTool', iface='Trig::TrigDecisionTool')
        if not self.tdt:
            self.msg.warning('Could not retrieve TrigDecisionTool pointer!')
            return StatusCode.Success

        # Set description variable in cut flow branch (default is trigger name, but can override in JO)
        self.setFilterDescription(self.trigger)

        ## Initialize event pass counters
        self.nProcessed        = 0
        self.nEventPassTrigger = 0 

        return StatusCode.Success


    def execute(self):

        if not self.tdt:
            self.msg.warning('Could not retrieve TrigDecisionTool pointer!')
            self.setFilterPassed(False)
            return StatusCode.Success

        isPassed = False
        
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...', self.name, self.nProcessed )
        

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true', self.name )
            self.setFilterPassed(True)
            return StatusCode.Success        

        #Check triggers
        #if self.tdt.isPassed(PyAthena.TrigDec.EF):
        if self.tdt.isPassed( self.trigger ):
            self.nEventPassTrigger += 1
            self.msg.debug("Trigger %s passed. Count: %d",
                           self.trigger, self.nEventPassTrigger)
            isPassed = True
            pass
        

        self.setFilterPassed(isPassed)
    
        return StatusCode.Success


    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiPassTrigger    = 0.0
        effiErrPassTrigger = 0.0

        try :
            print ('self.processed = ', self.nProcessed)
            
            # Efficiencies
            effiPassTrigger    = 100.*self.nEventPassTrigger / float(self.nProcessed)
            # Error on efficiencies
            effiErrPassTrigger = 100.*math.sqrt( self.nEventPassTrigger ) / float(self.nProcessed)
                        
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
        
        self.msg.info( '==> finalize %s...', self.name )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:', self.name )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r', self.nProcessed )
        self.msg.info( ' Events passing trigger %s:   %r and resulting efficiency = (%3.3f +/- %3.3f)%%',
                       self.trigger, self.nEventPassTrigger, effiPassTrigger, effiErrPassTrigger )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

