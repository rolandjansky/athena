# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        CompositeMissingEtFilter.py
##
## Author:      Karsten Koeneke (DESY)
## Created:     July 2009
##
## Based on:    LargeMetFilter.py by UllaBlumenschein
##
## Description: This is a short algorithm to select events that contain 
##              a certain amount of missing transverse energy. A minimum
##              deltaPhi cut between the missing Et and the leading jet
##              can be applied.
##
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain
a certain amount of missing transverse energy. A minimum deltaPhi cut
between the missing Et and the leading jet can be applied.
"""
__version__ = "0.0.1"
__author__  = "Ulla Blumenschein <Ulla.Blumenschein@cern.ch>"
__author__  = "Karsten Koeneke <karsten.koeneke@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class CompositeMissingEtFilter( PyAthena.AthFilterAlgorithm ):
    """
    This is a short algorithm to select events that contain
    a certain amount of missing transverse energy. A minimum deltaPhi cut
    between the missing Et and the leading jet can be applied.
    self.metCollectionTypeList = kw.get('metCollectionTypeList', [])
    self.metCollectionNameList = kw.get('metCollectionNameList', [])
    self.metCombinationMetric  = kw.get('metCombinationMetric', [])
    self.cutMetMin             = kw.get('cutMetMin', 0.0*Units.GeV)
    passAll              = False
    """



    def __init__ ( self, name = "CompositeMissingEtFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(CompositeMissingEtFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.metCollectionTypeList = kw.get('metCollectionTypeList', [])
        self.metCollectionNameList = kw.get('metCollectionNameList', [])
        self.metCombinationMetric  = kw.get('metCombinationMetric', [])
        self.cutMetMin             = kw.get('cutMetMin', -1000.0*Units.GeV)
        self.passAll               = kw.get('passAll', False)
        
        ## Also, declare some counters and initialize them to zero
        self.nProcessed         = 0
        self.nEventPassMet      = 0

        ## Get the storgate handle
        self.storeGateSvc = None

        return


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( '  metCollectionTypeList = %r', self.metCollectionTypeList )
        self.msg.info( '  metCollectionNameList = %r', self.metCollectionNameList )
        self.msg.info( '  metCombinationMetric  = %r', self.metCombinationMetric )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  cutMetMin             = %r', self.cutMetMin )
        self.msg.info( '************************************' )


        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure


        # Sanity check
        if len(self.metCollectionTypeList) != len(self.metCollectionNameList) \
           or len(self.metCollectionTypeList) != len(self.metCombinationMetric) :
            self.msg.error("Problem in setup. The lists have different lenghts! Please configure it properly...")
            return StatusCode.Failure

        
        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()
        

        # Calculate some values
        self.cutMetMinSquared = self.cutMetMin * self.cutMetMin

        return StatusCode.Success




    def execute(self):
        # Increment the event counter
        self.nProcessed+=1

        # Print a message to the log file
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        # Try to access container. Avoid crash if Containers absent in first data
        metList = []
        for i in xrange( len(self.metCollectionTypeList) ) :
            try:
                metList.append(self.storeGateSvc.retrieve( self.metCollectionTypeList[i], self.metCollectionNameList[i] ))
                pass
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.metCollectionNameList[i] )           
                self.setFilterPassed(True)
                return StatusCode.Success   
            pass # End: Loop over all input MET containers


        # Initialize some variables
        PassMet   = False
        metSqared = 0.0
        metx      = 0.0
        mety      = 0.0
        sumEt     = 0.0
        metric    = 1.0

        # Loop over all missingEt objects
        for idx in xrange(len(metList)) :
            # Get the current MET object and the metric to be used
            metObj = metList[idx]
            metric = self.metCombinationMetric[idx]
            # Do the combination
            metx  += metric * metObj.etx()
            mety  += metric * metObj.ety()
            sumEt += metric * metObj.sumet()
            pass # End: Loop over the met containers

        # Now, calculate the final missingEt
        metSquared = metx*metx + mety*mety


        # Now, do the selection
        if metSquared > self.cutMetMinSquared :
            PassMet = True
            pass
        
        
        ## Translate the jet pass counters into event pass counters
        if PassMet: self.nEventPassMet += 1


        ## Check if the event is accepted
        if PassMet :
            self.msg.debug( '%s event passed ' % self.name() )
            self.setFilterPassed(True)
        else:
            self.msg.debug( '%s event failed ' % self.name() )
            self.setFilterPassed(False)
            pass
        
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effPassMet     = 0.0
        efferrPassMet  = 0.0
        try :
            # Efficiencies
            effPassMet  = self.nEventPassMet / float(self.nProcessed)

            # Error on efficiencies, use binomial errors
            efferrPassMet  = 100.0*math.sqrt(effPassMet*(1.-effPassMet)/float(self.nProcessed))

            effPassMet  *= 100.0
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Events after Met cut:       %r, efficiency = (%3.3f +/- %3.3f)%%' %
                       ( self.nEventPassMet, effPassMet, efferrPassMet ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

