#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ShellJobRunner is a JobRunner for running one job at a time in current shell.

Written by Juerg Beringer (LBNL) in 2008.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: ShellJobRunner.py 216126 2009-09-29 16:12:59Z atlidbs $'


from JobRunner import JobRunner
import os


class ShellJobRunner(JobRunner):
    """ShellJobRunner - runs jobs one at a time in current shell"""

    def __init__(self,**params):
        """Constructor (takes any number of parameters as an argument)."""
        JobRunner.__init__(self,**params)

    def submitJob(self,jobConfig):
        """Run a JobRunner job in the current shell."""
        scriptfile = jobConfig['scriptfile']
        logfile = jobConfig['logfile']
        status = os.system(scriptfile+' > '+logfile+' 2>&1') >> 8   # Convert to standard Unix exit code 
        return status
