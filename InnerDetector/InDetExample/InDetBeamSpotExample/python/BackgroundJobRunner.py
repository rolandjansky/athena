#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
BackgroundJobRunner is a JobRunner for running jobs in the background.

Written by Juerg Beringer (LBNL) in 2008.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: BackgroundJobRunner.py 216126 2009-09-29 16:12:59Z atlidbs $'


from JobRunner import JobRunner
import os


class BackgroundJobRunner(JobRunner):
    """BackgroundJobRunner - run jobs in the background"""

    def __init__(self,**params):
        """Constructor (takes any number of parameters as an argument)."""
        JobRunner.__init__(self,**params)

    def submitJob(self,jobConfig):
        """Run a JobRunner job in the background."""
        scriptfile = jobConfig['scriptfile']
        logfile = jobConfig['logfile']
        os.system(scriptfile+' > '+logfile+' 2>&1 &')
        return None
