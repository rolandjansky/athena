# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Runs a single helloworld job in linux interactive mode."""

def run():
    import os

    # Setup the cmt home dir
    cmd  = 'cd cmtHome;'
    cmd += 'source /afs/cern.ch/sw/contrib/CMT/v1r20p20080222/mgr/setup.sh>& /dev/null;'
    cmd += 'cmt config >& /dev/null;'

    # Build the HW package
    cmd += 'source setup.sh -tag=opt,32,gcc34,AtlasProduction,14.5.0,dev,setup,oneTest >& /dev/null;'
    cmd += 'cd ../Control/AthenaExamples/AthExHelloWorld/cmt;'
    cmd += 'gmake >& /dev/null;'
    os.system(cmd)
