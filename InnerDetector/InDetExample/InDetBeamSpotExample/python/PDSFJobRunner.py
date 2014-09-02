#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
PDSFJobRunner is a JobRunner for running jobs with the SGE batch system on pdsf.nersc.gov.

Written by Juerg Beringer (LBNL) in 2009.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: PDSFJobRunner.py 343090 2011-02-01 00:39:37Z hurwitz $'


from JobRunner import JobRunner
import os
import time

preScript = """#!/bin/bash

#$ -hard
#$ -l h_cpu=24:00:00
#$ -l eliza4io=1

ulimit -c 0

echo "Starting on host $(hostname)  on  $(date)"
echo "JOB_ID=$JOB_ID"
echo ""
ulimit -a
echo ""

"""

class PDSFJobRunner(JobRunner):
    """PDSFJobRunner - run jobs using the SGE batch system on PDSF"""

    def __init__(self,**params):
        """Constructor (takes any number of parameters as an argument)."""
        JobRunner.__init__(self)

        # Set user-specified parameters only after PDSFJobRunner defaults
        self.setParam('batchqueue','all.64bit.q','Batch queue')
        script = self.getParam('script')
        self.setParam('script',preScript+script)
        for k in params.keys():
            self.setParam(k,params[k])
        # SGE runs jobs from where they were submitted, so change rundir explicitly to jobdir
        self.setParam('rundir','%(jobdir)s')
        self.setParam('maketmpdir','')
        self.checkParams()

    def submitJob(self,jobConfig):
        """Submit a JobRunner job as a SGE batch job."""
        #batchCmd = 'qsub -q %(batchqueue)s -N %(jobname)s -j y -o %(logfile)s %(scriptfile)s' % jobConfig
        batchCmd = 'qsub -N %(jobname)s -j y -o %(logfile)s %(scriptfile)s' % jobConfig
        print batchCmd
        time.sleep(5)
        os.system(batchCmd)
        return None
