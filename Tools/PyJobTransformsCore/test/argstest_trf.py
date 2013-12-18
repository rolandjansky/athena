#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *

class TestArgsJobTransform( JobTransform ):
      def __init__(self):
        JobTransform.__init__( self,
                               version='1.0.0',
                               authors=Author('Martin Woudstra', 'Martin.Woudstra@cern.ch'),
                               skeleton='skeleton.test.py',
                               help="""Test all JobTransform Argument classes""" )
        # add arguments
        self.add( RunNumberArg() )
        self.add( FirstEventArg() )
        self.add( MaxEventsArg() )
        self.add( SkipEventsArg() )
        self.add( RandomSeedArg('Test of randomseed') )
        self.add( JobOptionsArg() )
        self.add( InputEvgenFileArg() )
        self.add( OutputEvgenFileArg() )
        self.add( InputHitsFileArg() )
        self.add( OutputHitsFileArg() )
        self.add( InputRDOFileArg() )
        self.add( OutputRDOFileArg() )
        self.add( InputESDFileArg() )
        self.add( OutputESDFileArg() )
        self.add( InputAODFileArg() )
        self.add( OutputAODFileArg() )
        self.add( GeometryVersionArg() )
        self.add( JobConfigArg("theConfig") )
        self.add( InputGeneratorFileArg(), 'NONE' )
        self.add( HistogramFileArg(), 'NONE' )
        self.add( NtupleFileArg(), 'NONE' )

        # add utilities
        self.add( SQLiteSupport() )
        self.add( GetFiles( ["PDGTABLE.MeV"] ) )

# make transformation object              
trf = TestArgsJobTransform()
# execute it if not imported
if __name__ == '__main__': trf.exeSysArgs()




        
