#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
  
  
__doc__ = """Run pythia 6 validation monitoring"""
  

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *  
from PyJobTransformsCore.trfutil import *
  
from HepMCAnalysis_i.Pythia6TrfConfig import pythia6config
  
class Pythia6valid_trf( JobTransform ):

    def __init__(self):
        
        JobTransform.__init__(self,authors = [ Author('Some author', 'someauthor@cern.ch') ] ,skeleton='HepMCAnalysis_i/Pythia6skeleton.py' ,help = __doc__ ,config = pythia6config )

        #add arguments
        
        self.add( HistogramFileArg() )
        self.add( MaxEventsArg() )

    def doPreRunActions(self):
        JobTransform.doPreRunActions(self)

# execute it if not imported

if __name__ == '__main__':
    trf = Pythia6valid_trf()
    sys.exit(trf.exeSysArgs().exitCode())
    
