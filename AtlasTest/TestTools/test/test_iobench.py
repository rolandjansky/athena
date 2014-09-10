#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    test_iobench.py
# @purpose: unit test file for the iobench module
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    July 2006

import user
import sys
from TestTools    import iobench
from AthenaCommon import ChapPy

if __name__ == "__main__":
    print "#"*80
    print "## testing iobench ..."

    jobOptions = [
        ChapPy.JobOptionsCmd( "OUTPUT=\"/tmp/slimmed.aod.pool\"" ),
        ChapPy.JobOptions( "McParticleAlgs/test_WriteMcAod_jobOptions.py" )
        ]
    athena = ChapPy.Athena( jobOptions = jobOptions,
                            checkLeak  = True )
    athena.EvtMax = 100

    bench = iobench.AthBench( athena,
                              nTimes = 10 )
    print "## bench:"
    print bench.athena
    bench.run()

    bench.ioStats( [ "GEN_AOD", "GEN_EVENT", "SpclMC" ], "w" )
    bench.save( "iobench-%ievts.log" % athena.EvtMax )
    bench.stats()

    print ""
    print "## Bye."
    print "#"*80

    # a possible output...
    """
    ## stats : [GEN_AOD] (nbenchs = 10)
     time<User> =     1231.000 us/evt
     time<Sys > =        2.000 us/evt
     time<Real> =     1210.000 us/evt
    
    ## stats : [GEN_EVENT] (nbenchs = 10)
     time<User> =     1553.000 us/evt
     time<Sys > =        4.000 us/evt
     time<Real> =     1577.000 us/evt
    
    ## stats : [SpclMC] (nbenchs = 10)
     time<User> =     1457.000 us/evt
     time<Sys > =        4.000 us/evt
     time<Real> =     1560.000 us/evt

    ## stats : [Overall] (nbenchs = 10)
     time<User> =       26.931 s
     time<Sys > =        0.749 s
     time<Real> =       41.064 s
    """
