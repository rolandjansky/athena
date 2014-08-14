# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        LargeMET Skimmer
##
## Author:      Ulla Blumenschein (Goettingen)
## Created:     August 2008
##
## Based on:    PhotonFilter.py by Karsten Koeneke (DESY)
##
## Description: This is a short algorithm to select events that contain a jet
##              that fulfills a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
jet which fulfills a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "Ulla Blumenschein <Ulla.Blumenschein@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class LargeMetFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain MET which exceed a certain value.
    The preselection could contain a refinement of the MET definition, protection from fake MET:
    cutEtMin         = 30.0*Units.GeV
    doPreselection   = False
    passAll          = False
    requireMet       = True    
    """



    def __init__ ( self, name = "LargeMetFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(LargeMetFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.metCollectionType = kw.get('metCollectionType', "MissingET")
        self.metCollectionName = kw.get('metCollectionName', "MET_RefFinal")
        self.cutEtMin          = kw.get('cutEtMin', 30.0*Units.GeV)
        self.doPreselection    = kw.get('doPreselection', False)
        self.requireMet        = kw.get('requireMet', True)
        self.passAll           = kw.get('passAll', False)
        ## Also, declare some counters and initialize them to zero
        self.nProcessed       = 0
        self.nEventPassEt     = 0
        self.nEventPassPresel = 0
        ## Get the storgate handle
        self.storeGateSvc     = None



    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  EtMin            = %r', self.cutEtMin )
        self.msg.info( '  doPreselection    = %r', self.doPreselection )
        self.msg.info( '************************************' )


        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()
        
        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success




    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        PassEt      = False
        PassPresel  = False


        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success

        # Try to access container. Avoid crash if Containers absent in first data 
                
        try:
            met = self.storeGateSvc.retrieve( self.metCollectionType, self.metCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.metCollectionName )           
            if self.requireMet:
                self.setFilterPassed(False)
            else:
                self.setFilterPassed(True)
            return StatusCode.Success   
           


        if met.et()>  self.cutEtMin :
            PassEt  = True
            PassPresel = True # no preselection applied yet 
        pass

        

        ## Translate the jet pass counters into event pass counters
        if PassEt : self.nEventPassEt += 1
        if PassPresel  : self.nEventPassPresel += 1


        ## Check if the event is accepted
        if PassEt :
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
        effPassEt       = 0.0
        effPresel       = 0.0
        efferrPassEt    = 0.0
        efferrPresel    = 0.0        
        try :
            # Efficiencies
            effPassEt     = self.nEventPassEt / float(self.nProcessed)
            effPresel     = self.nEventPassPresel / float(self.nProcessed)

            # Error on efficiencies
            efferrPassEt    = 100.0*math.sqrt(effPassEt*(1.-effPassEt)/float(self.nProcessed))
            efferrPresel    = 100.0*math.sqrt(effPresel*(1.-effPresel)/float(self.nProcessed))

            effPassEt *= 100.
            effPresel *= 100.
            
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Events after E_T cut:       %r, efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEt, effPassEt, efferrPassEt ) )
        self.msg.info( ' Events after preselection:  %r, efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassPresel, effPresel, efferrPresel ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

