# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        SkimDecisionFilter
##
## Author:      Karsten Koeneke (DESY)
## Created:     December 2009
##
## Description: This is a short algorithm to select events that were accepted
##              by a certain event filter and bookkept into a SkimDecisionCollection.
##=============================================================================

__doc__ = """This is a short algorithm to select events that were accepted
by a certain event filter and bookkept into a SkimDecisionCollection.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class SkimDecisionFilter( PyAthena.Alg ):
    """This is a short algorithm to select events that were accepted
    by a certain event filter and bookkept into a SkimDecisionCollection.
    """

    def __init__ ( self, name = "SkimDecisionFilter", **kw ) :
        kw['name'] = name
        super( SkimDecisionFilter, self ).__init__(**kw)

        # Define the cuts
        self.SkimDecisionsContainerName = kw.get( 'SkimDecisionsContainer',  "SkimDecisionsContainer" )
        self.SkimDecisionsFilterName    = kw.get( 'SkimDecisionsFilterName', "" )
        pass
    

    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will use the following input container: %r', self.SkimDecisionsContainerName )

        self.sg = PyAthena.py_svc( "StoreGateSvc" )
        return StatusCode.Success


    def execute(self):
        sdc = self.sg.retrieve( "SkimDecisionCollection", self.SkimDecisionsContainerName )
        for sd in sdc:
            self.msg.debug( 'SkimDecision for filter %s was = %r' % ( sd.getName(), sd.isAccepted() ) )
            if sd.getName() == self.SkimDecisionsFilterName :
                self.setFilterPassed( sd.isAccepted() )
                return StatusCode.Success # Found SkimDecision
            pass
        # Did NOT find SkimDecision
        self.setFilterPassed( False )
        return StatusCode.Success
    
    
    def finalize(self):
        return StatusCode.Success
    

    pass
