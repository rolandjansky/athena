# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Single Muon Skimmer
##
## Author:      David Lopez Mateos (Columbia University/Caltech)
## Created:     July 2008
##
## Description: This is a short algorithm to select events that contain one
##              muon which fulfills a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
muon which fulfills a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "David Lopez Mateos <David.Lopez@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class MuonSegmentFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain some 
    number of muon segments (probably zero).
    """



    def __init__ ( self, name = "MuonSegmentFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(MuonSegmentFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.muonSegmentContainerName = kw.get('muonSegmentContainers', "MooreSegments")
        self.muonSegmentContainerType = kw.get('muonSegmentContainerType', "Trk::SegmentCollection")         
        self.passAll            = kw.get('passAll', False)
        self.minNumberSegments    = kw.get('minNumberSegments', 0)
        self.maxNumberSegments    = kw.get('maxNumberSegments', 0)        

        #handle to storegate
        self.sgSvc = None
        self.containerNames = []

        return

    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '===> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( 'Minimum number of segments = %r',str(self.minNumberSegments) )
        self.msg.info( 'Maximum number of segments = %r',str(self.maxNumberSegments) )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed            = 0
        self.nMuonSegments         = 0
        self.nEventPassed          = 0

        ## retrieve a handle to the event store
        self.sgSvc = PyAthena.py_svc('StoreGateSvc')
        if not self.sgSvc :
            self.msg.error("could not retrieve a handle to the event store !")
            return StatusCode.Failure

        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success

        jetCollection = None
        _retrieve = self.sgSvc.retrieve

        try :
            muSegCollection = _retrieve (self.muonSegmentContainerType, self.muonSegmentContainerName)            
        except LookupError :
            self.msg.warning('%s container not found in StoreGate ' % self.muonSegmentContainerName)
   
        ## Check if the event is accepted
        if muSegCollection.__len__() >=  self.minNumberSegments and \
             muSegCollection.__len__()<=  self.maxNumberSegments  :
            self.nEventPassed += 1
            self.msg.debug( '%s event passed' % self.name() )
            self.setFilterPassed(True)
            pass
        else :
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
            pass

        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        effiNumPassed = 0.0
        effiErrNumPassed = 0.0        
        try :
            # Efficiencies
            effiNumPassed = self.nEventPassed / float(self.nProcessed)
            # Error on efficiencies, use binomial errors
            effiErrNumPassed = 100.0 * math.sqrt( effiNumPassed*(1.0 - effiNumPassed) / float(self.nProcessed) )
            effiNumPassed *= 100.0

        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Events after numberPassed %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassed, effiNumPassed, effiErrNumPassed ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success


