# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Creation: David Cote (DESY), November 2008

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class SkimDecisionsReader( PyAthena.Alg ):

    def __init__ ( self, name = "SkimDecisionsReader", **kw ) :
        kw['name'] = name
        super(SkimDecisionsReader,self).__init__(**kw)
        self.SkimDecisionsContainerName = kw.get( 'SkimDecisionsContainerName',  "SkimDecisionsContainer" )

    def initialize(self):
        self.sg = PyAthena.py_svc("StoreGateSvc")
        return StatusCode.Success

    def execute(self):
        try:
            sdc = self.sg.retrieve( "SkimDecisionCollection", self.SkimDecisionsContainerName)
        except LookupError:
            self.msg.warning( 'SkimDecisionCollection %s not found' % self.SkimDecisionsContainerName )           
            return StatusCode.Success
        self.msg.debug( "Retrieved SkimDecisionsCollection with name %s and lenght %s"  % ( self.SkimDecisionsContainerName, sdc.size() ) )
        
        for sd in sdc:
            self.msg.info( "Skim decision name:  %s", sd.getName() )
            self.msg.info( "Skim decision value: %s", sd.isAccepted() )
        return StatusCode.Success
            
    def finalize(self):
        return StatusCode.Success
