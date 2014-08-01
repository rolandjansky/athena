#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = """Run the streaming test."""

from PyJobTransformsCore.trf import JobTransform
from PyJobTransformsCore.full_trfarg import *
from RecJobTransforms.streaming_arg import *
from RecJobTransforms.MixStreamConfig import *

class StreamMixingTransform(JobTransform):    
    def __init__(self):
        JobTransform.__init__(self,
                              authors=[ Author('Ayana Holloway', 'ATHolloway@lbl.gov') ],
                              skeleton='RecJobTransforms/skeleton_mixing_streamtest.py',
                              help=__doc__,
                              config=mixStreamConfig )

        # Add runargs in order
        self.add ( JobConfigArg(package='RecJobTransforms'))
        self.add ( PartitionArg() )
        self.add ( MaxEventsArg())
        self.add ( RandomSeedArg('Seed for MixingEventSelector') )
        self.add ( OutputRDOFileArg(name='theRDOFile'), default="test.pool.root" )
        self.add ( OutputRDOFileArg(name='egammaRDOFile'), default="NONE" )
        self.add ( OutputRDOFileArg(name='muonRDOFile'), default="NONE" )
        self.add ( OutputRDOFileArg(name='jetRDOFile'), default="NONE" )
        self.add ( OutputRDOFileArg(name='expressRDOFile'), default="NONE" )        
        self.add ( TriggerConfigArg(), default='lumi1E31_no_prescale' )
        self.add ( GeometryVersionArg(), default='CSC-ATLAS-01-02-00' )


    def writeRunArgs(self):
        #First clone old trf behavior
        JobTransform.writeRunArgs(self);

        #Now make a new command file for including.
        f = file("./streamtest_mixer_partition_args.py" ,'w')
        f.write( "#Derived run config from PartitionArgs\n" )
        f.write( self.getArgument("partition").WritePartitionJobOptions() )
        f.close()
        
# execute it if run as a script
if __name__ == '__main__':
    # make transform object
    trf = StreamMixingTransform()
    import sys
    sys.exit( trf.exeSysArgs().exitCode() )
