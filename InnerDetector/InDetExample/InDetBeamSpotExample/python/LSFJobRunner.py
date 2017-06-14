#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
LSFJobRunner is a JobRunner for running jobs with the LSF batch system.

Written by Juerg Beringer (LBNL) in 2008.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: LSFJobRunner.py 216126 2009-09-29 16:12:59Z atlidbs $'


from JobRunner import JobRunner
import os


class LSFJobRunner(JobRunner):
    """LSFJobRunner - run jobs using the LSF batch system"""

    def __init__(self,**params):
        """Constructor (takes any number of parameters as an argument)."""
        JobRunner.__init__(self)

        # Set user-specified parameters only after LSFJobRunner defaults
        self.setParam('batchqueue','8nm','Batch queue')
        for k in params.keys():
            self.setParam(k,params[k])
        self.checkParams()

    def submitJob(self,jobConfig):
        """Submit a JobRunner job as a LSF batch job."""
        batchCmd = 'bsub -L /bin/bash -q %(batchqueue)s -J %(jobname)s -o %(logfile)s %(scriptfile)s' % jobConfig
        print batchCmd
        os.system(batchCmd)
        return None
